#include <volt/pch.hpp>
#include <volt/ecs/ecs.hpp>

#include <volt/ecs/system.hpp>

namespace volt::ecs::_internal {

std::unordered_map<std::type_index, size_t> component_type_index_to_index;
std::unordered_map<std::type_index, size_t> system_type_index_to_index;

std::unordered_map<std::string, size_t> component_name_to_index;
std::unordered_map<std::string, size_t> system_name_to_index;

std::vector<std::function<base_component_storage *
		()>> component_index_to_storage_constructor;
std::vector<std::function<system *
		()>> system_index_to_constructor;

#ifdef VOLT_DEVELOPMENT

std::unordered_map<std::string, std::vector<std::string>> module_name_to_component_names;
std::unordered_map<std::string, std::vector<std::string>> module_name_to_system_names;

#endif



size_t component_count() {
	return component_type_index_to_index.size();
}

size_t system_count() {
	return system_type_index_to_index.size();
}

#ifdef VOLT_DEVELOPMENT

const std::string &component_name(size_t index) {
	return std::find_if(
		component_name_to_index.begin(), component_name_to_index.end(),
		[&] (auto &item) {
			return item.second == index;
		}
	)->first;
}

const std::string &system_name(size_t index) {
	return std::find_if(
		system_name_to_index.begin(), system_name_to_index.end(),
		[&] (auto &item) {
			return item.second == index;
		}
	)->first;
}

void development_module_load_callback() {
	// Recover state
	if (module_name_to_component_names.contains(VOLT_DEVELOPMENT_MODULE))
		world::_register_development_components();
	
	world::_clear_development_reload_snapshots();
}

void development_module_unload_callback() {
	// There's no point in fixing discontinuities in component indexing,
	// because we always remove all tail components
	// which are owned by the development module.

	// Clean up components
	if (module_name_to_component_names.contains(VOLT_DEVELOPMENT_MODULE)) {
		// Save state
		world::_unregister_development_components();

		size_t min_index = component_index_to_storage_constructor.size();

		for (auto &name : module_name_to_component_names[VOLT_DEVELOPMENT_MODULE]) {
			size_t index = component_name_to_index[name];

			// Erase component from component_type_index_to_index
			component_type_index_to_index.erase(std::find_if(
				component_type_index_to_index.begin(),
				component_type_index_to_index.end(),
				[&](auto &item) {
					return item.second == index;
				}
			));

			// Erase component from component_name_to_index
			component_name_to_index.erase(name);

			// For erasing storage constructors
			if (index < min_index)
				min_index = index;
		}
		// Erase storage constructors
		component_index_to_storage_constructor.resize(min_index);

		// Erase module
		module_name_to_component_names.erase(VOLT_DEVELOPMENT_MODULE);
	}

	// Clean up systems
	if (module_name_to_system_names.contains(VOLT_DEVELOPMENT_MODULE)) {
		size_t min_index = system_index_to_constructor.size();

		for (auto &name : module_name_to_system_names[VOLT_DEVELOPMENT_MODULE]) {
			size_t index = system_name_to_index[name];

			// Erase system from system_type_index_to_index
			system_type_index_to_index.erase(std::find_if(
				system_type_index_to_index.begin(),
				system_type_index_to_index.end(),
				[&](auto &item) {
					return item.second == index;
				}
			));

			// Erase system from system_name_to_index
			system_name_to_index.erase(name);

			// For erasing constructors
			if (index < min_index)
				min_index = index;
		}

		// Erase constructors
		system_index_to_constructor.resize(min_index);
		
		// Erase module
		module_name_to_system_names.erase(VOLT_DEVELOPMENT_MODULE);

		// Delete all systems
		// They will be re-instantiated on first update
		// and query new component indices for lock masks
		system::_delete_all();
	}
}

#endif

void unload_all_modules_callback() {
	VOLT_ASSERT(world::_instances.empty(), "All worlds must be destroyed before unloading modules.")

	component_type_index_to_index.clear();
	system_type_index_to_index.clear();

	component_name_to_index.clear();
	system_name_to_index.clear();

	component_index_to_storage_constructor.clear();
	system_index_to_constructor.clear();

#ifdef VOLT_DEVELOPMENT
	module_name_to_component_names.clear();
	module_name_to_system_names.clear();
#endif

	system::_delete_all();
}

}
