#include <volt/pch.hpp>
#include <volt/ecs/system.hpp>

#include <volt/math/math.hpp>
#include <volt/log.hpp>

namespace volt::ecs {

using namespace _internal;

class simulated_thread_pool {
public:
	void submit(float duration, std::function<void()> finished_callback = {}) {
        queue.push({ duration, finished_callback });
    }

    float simulate(uint32_t thread_count) {
        float total_duration = 0;
        std::vector<float> thread_occupation(thread_count, 0); // 0 = not running, otherwise the thread running

        // Find the thread with shortest occupation time and add a duration to it
        while (!queue.empty()) {
            auto item = std::move(queue.front());
            queue.pop();

            float &min_occupation = *std::min_element(thread_occupation.begin(), thread_occupation.end());
            min_occupation += item.first;

            // Realistically jobs would run in semi-random order, but running them in the order of submission is a decent approximation
            item.second();
        }
        
        // The longest occupation time is the total execution time
        return *std::max_element(thread_occupation.begin(), thread_occupation.end());
    }

private:
	std::queue<std::pair<float, std::function<void()>>> queue;
};

class simulation {
public:
    simulated_thread_pool pool;
    std::set<size_t> running_systems;
    std::vector<size_t> sorted_systems_to_run;
};

constexpr float profiling_lerp_factor = 0.5F;

void system::_update_all(float delta) {
    // There is no instances on first run or after a development reload
	if (instances.empty()) {
		size_t count = system_count();
		instances.reserve(count);

		// Construct systems
		for (size_t i = 0; i < count; i++) {
			// Construction must happen before insertion to system_instances, because
			// constructors might use the size of system_instances to find their own index.
			ecs::system *system = system_index_to_constructor[i]();
            // Used during topological sorting
            system->unfulfilled_dependency_count = system->_after.size();
			instances.emplace_back(system);
		}

        // Populate "before" vectors (by inserting "after" dependencies) and find root systems
        // This can carry over redundant connections if they were present, but
        // they are easy to fix after emitting the warning if redundancy is detected
        // We need to populate before we can sort the systems topologically to detect circular dependencies
        // and we need to detect circular dependencies before we can detect redundancy to avoid the infinite loop
		for (size_t i = 0; i < count; i++) {
			std::vector<size_t> &after = instances[i]->_after;
			if (after.empty())
				root_systems.emplace_back(i);
			else for (size_t j : after)
				instances[j]->before.emplace_back(i);
		}

        // Sort topologically using Kahn's algorithm
        std::vector<size_t> sorted_systems;
        sorted_systems.reserve(count);
        {
            std::queue<size_t> systems_to_sort;
            for (size_t i : root_systems)
                systems_to_sort.push(i);

            while (!systems_to_sort.empty()) {
                size_t i = systems_to_sort.front();
                systems_to_sort.pop();
                sorted_systems.emplace_back(i);

                for (size_t j : instances[i]->before) {
                    system &sys = *instances[j];
                    if (sys.unfulfilled_dependency_count.fetch_sub(1) == 1)
                        systems_to_sort.push(j);
                }
            }

            // Check if there were circular dependencies
            if (sorted_systems.size() != count) {
                // This is a terminal block, VOLT_ASSERT will be always reached
                std::string msg = "System graph has a circular dependency. Participating systems:";
                for (size_t i = 0; i < count; i++) {
                    if (std::find(sorted_systems.begin(), sorted_systems.end(), i) == sorted_systems.end())
                        msg += " " + system_name(i);
                }
                VOLT_ASSERT(false, msg)
            }
        }

#ifdef VOLT_DEVELOPMENT
        // Disallow redundant dependencies
        // We had to first check for circular dependencies during topological sorting

        // For each system
        for (size_t i = 0; i < count; i++) {
            // Get that system
            system &cur_sys = *instances[i];

            // Check all systems it depends on
            std::stack<size_t> systems_to_check;
            for (size_t j : cur_sys._after)
                systems_to_check.push(j);

            while (!systems_to_check.empty()) {
                size_t j = systems_to_check.top();
                systems_to_check.pop();

                system &chk_sys = *instances[j];

                // Check if the system we're checking (chk_sys) is dependent
                // on any dependency of the current system (cur_sys)
                for (size_t k : chk_sys._after) {
                    auto it = std::find(cur_sys._after.begin(), cur_sys._after.end(), k);
                    if (it != cur_sys._after.end()) {
                        system &dep_sys = *instances[k];

                        VOLT_LOG_WARNING("Dependency of system " + system_name(i) + " on system " + system_name(k)
                                + " is implicitly declared because system " + system_name(j) + " also depends on " + system_name(k)
                                + ". Dependency will be skipped. Please remove redundant code.");
                        
                        cur_sys._after.erase(it);
                        std::remove(dep_sys.before.begin(), dep_sys.before.end(), i);
                    }
                }

                // Also check dependencies of those dependant systems
                for (size_t k : chk_sys._after)
                    systems_to_check.push(j);
            }
        }
#endif

        // Profile single thread system performance
        // (Also update systems for this frame)
        std::vector<float> system_durations(count);
        {
            util::timer profiling_timer;
            for (size_t i : sorted_systems) {
                profiling_timer.reset();
                instances[i]->update(delta);
                system_durations[i] = static_cast<float>(profiling_timer.elapsed());
            }
        }

        // Sort systems by descending duration
        std::vector<size_t> systems_sorted_by_duration(count);
        std::iota(systems_sorted_by_duration.begin(), systems_sorted_by_duration.end(), 0);
        std::sort(
            systems_sorted_by_duration.begin(),
            systems_sorted_by_duration.end(),
            [&](size_t a, size_t b) {
                return system_durations[a] > system_durations[b];
            }
        );

        // Simulate multi-threaded performance for different
        // topological orderings to select the best one

        // Start executing systems in the order of descending duration
        // Whenever a system finishes and multiple can start (moment A), we attempt to run them by starting from a different system on each iteration
        // Running systems in different order might prevent some of them from runnning because not all of them can run in parallel
        // Each time we try a new starting system, we use the duration order for the upcoming similar situations and measure the overall performance
        // We finally order the systems that can be run in moment A by the overall performance that starting with each one of them gives
        
        size_t thread_count = util::thread_pool::common_pool.thread_count();

        std::function<void(simulation &)> continue_simulation = [&](simulation &simulation) {
            // Precompute discrimination masks
            util::dynamic_bitset unique_mask, shared_mask;
            for (size_t j : simulation.running_systems) {
                unique_mask |= instances[j]->unique_mask;
                shared_mask |= instances[j]->shared_mask;
            }
            
            auto it = simulation.sorted_systems_to_run.begin();
            while (it != simulation.sorted_systems_to_run.end()) {
                size_t i = *it;
                system &system = *instances[i];

                // Determine if this system can be started
                bool can_run_in_parallel =
                        (system.unique_mask & shared_mask).all(false) &&
                        (unique_mask & system.shared_mask).all(false);
                if (!can_run_in_parallel)
                    continue;

                // Submit the system for starting
                simulation.pool.submit(system_durations[i], [&]() {
                    // After this system finishes:

                    // Remove it from the running systems list
                    simulation.running_systems.erase(i);

                    // Add next systems to sorted_systems_to_run queue
                    for (size_t j : system.before) {
                        ecs::system &next_system = *instances[j];

                        if (next_system.unfulfilled_dependency_count.fetch_sub(1) == 1) {
                            simulation.sorted_systems_to_run.emplace_back(j);
                            simulation.sorted_systems_to_run.insert(std::upper_bound(
                                simulation.sorted_systems_to_run.begin(), simulation.sorted_systems_to_run.end(),
                                        j, [&](size_t a, size_t b) {
                                    return system_durations[a] > system_durations[b];
                                }), j);
                        
                            // This system won't appear again in this frame so we can reset its counter
                            next_system.unfulfilled_dependency_count = next_system._after.size();
                        }
                    }

                    // Continue after queueing new systems up for execution
                    continue_simulation(simulation);
                });

                // Remove the system from the queue and mark it as running
                it = simulation.sorted_systems_to_run.erase(it);
                simulation.running_systems.insert(i);
                // Update discrimination masks
                unique_mask |= system.unique_mask;
                shared_mask |= system.shared_mask;
                // This update is valid because we're only adding systems to the queue during the while loop
            }
        };

        std::function<void(simulation &)> simulate_all = [&](simulation &simulation) {
            // Try to start with different systems
            std::unordered_map<size_t, float> starting_system_to_total_duration;

            for (size_t i = 0; i < simulation.sorted_systems_to_run.size(); i++) {
                std::vector<size_t> custom_ordering;
                custom_ordering.reserve(simulation.sorted_systems_to_run.size());

                // Execute i-th system first
                custom_ordering.emplace_back(simulation.sorted_systems_to_run[i]);

                // Then start every other system
                size_t systems_that_have_been_run = 1;
                for (size_t j = 0; j < simulation.sorted_systems_to_run.size(); j++) {
                    if (i != j) {
                        custom_ordering.emplace_back(simulation.sorted_systems_to_run[j]);
                        systems_that_have_been_run++;
                    }
                }

                // No need to try other permutations if all of the systems were succesfully started
                if (systems_that_have_been_run == systems_that_can_run.size())
                    break;
            }
        };

        simulation simulation;
        simulation.sorted_systems_to_run = root_systems;
        std::sort(
            simulation.sorted_systems_to_run.begin(),
            simulation.sorted_systems_to_run.end(),
            [&](size_t a, size_t b) {
                return system_durations[a] > system_durations[b];
            }
        );
        
        // std::vector<size_t> best_ordering = simulate_all(simulation);

        // Based on the final topological ordering, insert additional dependencies between systems that cannot run in parallel
        for (int32_t i = count - 2; i >= 0; i--) { // Signed type is needed
            size_t index1 = sorted_systems[i];
            system &sys1 = *instances[index1];

            for (size_t j = i + 1; j < count; j++) {
                size_t index2 = sorted_systems[j];
                system &sys2 = *instances[index2];

                // Check if sys1 and sys2 can run in parallel
                bool can_run_in_parallel =
                        (sys1.unique_mask & sys2.shared_mask).all(false) &&
                        (sys2.unique_mask & sys1.shared_mask).all(false);
                if (can_run_in_parallel)
                    continue;

                // Check if sys1 is dependent on sys2 or vice versa
                if (is_dependent_on(i, j) || is_dependent_on(j, i))
                    continue;
            
                // Finally, if those systems cannot run in parallel and there is no dependency present between them
                sys1.before.emplace_back(index2);
                sys2._after.emplace_back(index1); // This is needed for is_dependent_on to work
            }
        }
	} else {
        std::function<std::shared_ptr<util::future>(system &sys)> async_propagate_system_update = [&](system &sys) {
            return util::thread_pool::common_pool.submit([&](uint32_t thread_index) {
                sys.update(delta);

                std::vector<std::shared_ptr<util::future>> futures;
                futures.reserve(sys.before.size());

                for (size_t i : sys.before) {
                    system &next_system = *instances[i];
                    if (next_system.unfulfilled_dependency_count.fetch_sub(1) == 1) {
                        futures.emplace_back(async_propagate_system_update(next_system));
                        
                        // This system won't appear again in this frame so we can reset its counter
                        next_system.unfulfilled_dependency_count = next_system._after.size();
                    }
                }

                for (auto &future : futures) 
                    future->rethrow(); // Waits
            });
        };

        std::vector<std::shared_ptr<util::future>> futures;
        futures.reserve(root_systems.size());

        // Update systems for this frame
        for (size_t i : root_systems)
            futures.emplace_back(async_propagate_system_update(*instances[i]));

        for (auto &future : futures)
            future->rethrow(); // Waits
    }
}

void system::_delete_all() {
    instances.clear();
    root_systems.clear();
}

void system::unique_lock_all() {
#ifdef VOLT_DEVELOPMENT
    // lock_impl detects user errors
    for (size_t i = 0; i < component_count(); i++)
        lock_impl<true>(*this, i);
#else
    unique_mask.set();
#endif
}

void system::shared_lock_all() {
#ifdef VOLT_DEVELOPMENT
    // lock_impl detects user errors
    for (size_t i = 0; i < component_count(); i++)
        lock_impl<false>(*this, i);
#else
    shared_mask.set();
#endif
}

void system::use_main_thread() {
    _use_main_thread = true;
}

std::vector<std::unique_ptr<system>> system::instances;
std::vector<size_t> system::root_systems;

bool system::is_dependent_on(size_t dependant, size_t dependency) {
    // Check all systems dependant depends on
    std::stack<size_t> systems_to_check;
    systems_to_check.push(dependant);

    while (!systems_to_check.empty()) {
        size_t j = systems_to_check.top();
        systems_to_check.pop();

        if (j == dependency)
            return true;

        system &dependency_sys = *instances[j];

        // Also check dependencies of those systems
        for (size_t k : dependency_sys._after)
            systems_to_check.push(j);
    }

    return false;
}

}