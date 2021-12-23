#include <volt/pch.hpp>
#include <volt/ecs/ecs.hpp>

#include <volt/ecs/world.hpp>
#include <volt/ecs/system.hpp>

namespace volt::ecs::_internal {

using unique_mask = std::bitset<VOLT_MAX_COMPONENTS>;
using shared_counter = std::array<uint16_t, VOLT_MAX_COMPONENTS>;

static unique_mask system_update_unique_mask;
static shared_counter system_update_shared_counter{};
static std::mutex system_update_mutex;
static std::condition_variable system_update_cv;

static unique_mask operator&(const unique_mask &mask,
		const shared_counter &counter) {
	unique_mask result;
	for (size_t i = 0; i < VOLT_MAX_COMPONENTS; i++)
		result[i] = mask[i] && counter[i];
	
	return result;
}

static shared_counter &operator+=(shared_counter &counter,
		const unique_mask &mask) {
	for (size_t i = 0; i < VOLT_MAX_COMPONENTS; i++)
		counter[i] += mask.test(i);
	
	return counter;
}

static shared_counter &operator-=(shared_counter &counter,
		const unique_mask &mask) {
	for (size_t i = 0; i < VOLT_MAX_COMPONENTS; i++)
		counter[i] -= mask.test(i);
	
	return counter;
}

}

namespace volt::ecs {

using namespace _internal;

size_t component_count() {
	return component_type_index_to_name.size();
}

const std::unordered_map<std::string, std::set<std::string>> &component_names() {
	return module_name_to_component_names;
}

const std::unordered_map<std::string, std::set<std::string>> &system_names() {
	return module_name_to_system_names;
}

void update_systems(float delta) {
	std::set<std::type_index> processed;

	for (auto &item : system_instances) {
		// Construct any new systems
		if (!item.second)
			item.second.reset(system_name_to_constructor[item.first]());

		// Update non-async
		if (!item.second->_async) {
			item.second->update(delta);
			processed.emplace(typeid(*item.second));
		}
	}

	std::unique_lock lock(system_update_mutex);

	auto it = system_instances.begin();
	while (processed.size() != system_instances.size()) {
		std::type_index type_index = typeid(*it->second);
		if (!processed.contains(type_index)) {
			auto system = static_cast<async_system *>(it->second.get());

			if (std::all_of(
					system->_dependencies.begin(),
					system->_dependencies.end(),
					[&processed](auto &dependency) {
						return processed.contains(dependency);
					})) {
				if ((system->_unique_mask & system_update_unique_mask).none()
						&& (system->_unique_mask & system_update_shared_counter).none()
						&& (system->_shared_mask & system_update_unique_mask).none()) {
					system_update_unique_mask |= system->_unique_mask;
					system_update_shared_counter += system->_shared_mask;

					// TODO Use custom thread pool
					std::thread([system, delta]() {
						system->update(delta);

						{
							std::unique_lock lock(system_update_mutex);
							system_update_unique_mask &= ~system->_unique_mask;
							system_update_shared_counter -= system->_shared_mask;
						}

						system_update_cv.notify_one();
					}).detach();

					processed.emplace(type_index);
				} // If components unlocked
			} // If deps fulfilled
		} // If not processed

		if (++it == system_instances.end()) {
			it = system_instances.begin();
			system_update_cv.wait(lock);
		}
	}
}

}

namespace volt::ecs::_internal {

std::unordered_map<std::type_index, std::string> component_type_index_to_name;
std::unordered_map<std::type_index, std::string> system_type_index_to_name;

std::unordered_map<std::string, size_t> component_name_to_index;
std::unordered_map<std::type_index, size_t> component_type_index_to_index;

std::unordered_map<std::string, std::function<base_component_storage *
		()>> component_name_to_storage_constructor;
std::unordered_map<std::string, std::function<system *
		()>> system_name_to_constructor;

std::unordered_map<std::string, std::unique_ptr<system>> system_instances;

std::unordered_map<std::string, std::set<std::string
		>> module_name_to_component_names;
std::unordered_map<std::string, std::set<std::string
		>> module_name_to_system_names;

size_t component_index(const std::string &name) {
	VOLT_DEVELOPMENT_ASSERT(component_name_to_index.contains(name),
			"No such component name registered: " + name)
	return component_name_to_index[name];
}

#ifdef VOLT_DEVELOPMENT

void module_load_callback(const std::string &module_name) {
	// There's no point in handling other modules,
	// because they won't ever be reloaded and
	// their state will never have to be recovered
	if (module_name != VOLT_DEVELOPMENT_MODULE)
		return;

	for (auto &name : module_name_to_component_names[VOLT_DEVELOPMENT_MODULE])
		world::_register_development_component(name);

	world::_clear_development_reload_snapshots();
}

#endif

void module_unload_callback(const std::string &module_name) {
	for (auto &name : module_name_to_component_names[module_name]) {
#ifdef VOLT_DEVELOPMENT
		if (module_name == VOLT_DEVELOPMENT_MODULE)
			world::_unregister_development_component(name);
#endif

		size_t index = component_name_to_index[name];
		size_t last_index = component_count() - 1;

		// Erase name from component_type_index_to_name
		auto type_index_it = std::find_if(
			component_type_index_to_name.begin(),
			component_type_index_to_name.end(),
			[&name](auto &item) {
				return item.second == name;
			}
		);
		std::type_index type_index = type_index_it->first;
		component_type_index_to_name.erase(type_index_it);

		// Erase component from component_name_to_index and change
		// last component index to one that's being removed
		component_name_to_index.erase(name);
		{
			auto it = std::find_if(
				component_name_to_index.begin(),
				component_name_to_index.end(),
				[last_index](auto &item) {
					return item.second == last_index;
				}
			);
			
			if (it != component_name_to_index.end())
				it->second = index;
		}

		// Erase component from component_type_index_to_index and change
		// last component index to one that's being removed
		component_type_index_to_index.erase(type_index);
		{
			auto it = std::find_if(
				component_type_index_to_index.begin(),
				component_type_index_to_index.end(),
				[last_index](auto &item) {
					return item.second == last_index;
				}
			);

			if (it != component_type_index_to_index.end())
				it->second = index;
		}

		// Erase constructor
		component_name_to_storage_constructor.erase(name);
	}

	module_name_to_component_names.erase(module_name);

	for (auto &name : module_name_to_system_names[module_name]) {
		system_type_index_to_name.erase(std::find_if(
			system_type_index_to_name.begin(),
			system_type_index_to_name.end(),
			[&name](auto &item) {
				return item.second == name;
			}
		));

		system_name_to_constructor.erase(name);

		system_instances.erase(name);
	}

	// Delete all systems
	// They will be re-instantiated on first update
	// and query new component indices for lock masks
	for (auto &instance : system_instances)
		instance.second = nullptr;
	
	module_name_to_system_names.erase(module_name);
}

}
