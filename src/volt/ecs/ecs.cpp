#include <volt/pch.hpp>
#include <volt/ecs/ecs.hpp>

#include <volt/ecs/world.hpp>
#include <volt/ecs/system.hpp>

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

namespace volt::ecs {

std::map<std::type_index, std::string> _component_type_index_to_name;

std::map<std::string, size_t> _component_name_to_index;
std::map<std::type_index, size_t> _component_type_index_to_index;

std::map<std::string, std::function<_base_component_storage *
		()>> _component_name_to_storage_constructor;
std::map<std::string, std::function<system *
		()>> _system_name_to_constructor;

std::map<std::string, std::unique_ptr<system>> _system_instances;

std::map<std::string, std::set<std::string
		>> _module_name_to_component_names;
std::map<std::string, std::set<std::string
		>> _module_name_to_system_names;

void _module_unload_callback(const std::string &module_name, bool remember_state) {
	world::_clear_state_snapshot(module_name);

	for (auto &name : _module_name_to_component_names[module_name]) {
		world::_unregister_component(name, remember_state, module_name);

		size_t index = _component_name_to_index[name];
		size_t last_index = get_component_count() - 1;

		std::unique_ptr<std::type_index> type_index;

		// Erase name from _component_type_index_to_name
		_component_type_index_to_name.erase(std::find_if(
			_component_type_index_to_name.begin(),
			_component_type_index_to_name.end(),
			[&name, &type_index](auto &item) {
				type_index.reset(new std::type_index(item.first));
				return item.second == name;
			}
		));

		// Erase component from _component_name_to_index and change
		// last component index to one that's being removed
		_component_name_to_index.erase(name);
		{
			auto it = std::find_if(
				_component_name_to_index.begin(),
				_component_name_to_index.end(),
				[last_index](auto &item) {
					return item.second == last_index;
				}
			);
			
			if (it != _component_name_to_index.end())
				it->second = index;
		}

		// Erase component from _type_index_to_index and change
		// last component index to one that's being removed
		_component_type_index_to_index.erase(*type_index);
		{
			auto it = std::find_if(
				_component_type_index_to_index.begin(),
				_component_type_index_to_index.end(),
				[last_index](auto &item) {
					return item.second == last_index;
				}
			);

			if (it != _component_type_index_to_index.end())
				it->second = index;
		}

		// Erase constructor
		_component_name_to_storage_constructor.erase(name);
	}

	_module_name_to_component_names.erase(module_name);

	for (auto &name : _module_name_to_system_names[module_name]) {
		_system_instances.erase(name);
		_system_name_to_constructor.erase(name);
	}

	// Delete all systems
	// They will be reinstantiated on first update
	// and query new component indices for lock masks
	for (auto &instance : _system_instances)
		instance.second = nullptr;
	
	_module_name_to_system_names.erase(module_name);
};

size_t get_component_count() {
	return _component_type_index_to_name.size();
}

const std::map<std::string, std::set<std::string>> &get_component_names() {
	return _module_name_to_component_names;
}

const std::map<std::string, std::set<std::string>> &get_system_names() {
	return _module_name_to_system_names;
}

void update_systems(float delta) {
	auto it = _system_instances.begin();
	auto end = _system_instances.end();

	std::unique_lock lock(system_update_mutex);
	while (end != _system_instances.begin()) {
		if (!it->second)
			it->second.reset(_system_name_to_constructor[it->first]());
		auto system = it->second.get();

		if ((system->_unique_mask & system_update_unique_mask).none()
				&& (system->_unique_mask & system_update_shared_counter).none()
				&& (system->_shared_mask & system_update_unique_mask).none()) {
			system_update_unique_mask |= system->_unique_mask;
			system_update_shared_counter += system->_shared_mask;

			// TODO Use custom thread pool
			std::thread([&end, system, delta]() {
				system->update(delta);

				{
					std::unique_lock lock(system_update_mutex);
					system_update_unique_mask &= ~system->_unique_mask;
					system_update_shared_counter -= system->_shared_mask;
					end--;
				}

				system_update_cv.notify_one();
			}).detach();
		}

		if (++it == end) {
			it = _system_instances.begin();
			system_update_cv.wait(lock);
		}
	}
}

}
