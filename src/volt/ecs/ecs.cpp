#include <volt/pch.hpp>
#include <volt/ecs/ecs.hpp>

namespace volt::ecs {

const std::unordered_map<std::string, std::vector<std::string>> &component_names() {
	return _internal::module_name_to_component_names;
}

const std::unordered_map<std::string, std::vector<std::string>> &system_names() {
	return _internal::module_name_to_system_names;
}

}

namespace volt::ecs::_internal {

std::unordered_map<std::string, std::vector<std::string>> module_name_to_component_names;
std::unordered_map<std::string, std::vector<std::string>> module_name_to_system_names;

std::unordered_map<std::type_index, size_t> component_type_index_to_index;
std::unordered_map<std::string, size_t> component_name_to_index;

std::vector<std::function<base_component_storage *
		()>> component_index_to_storage_constructor;

size_t component_count() {
	return component_type_index_to_index.size();
}

#ifdef VOLT_DEVELOPMENT

void development_module_load_callback() {
	if (module_name_to_component_names.contains(VOLT_DEVELOPMENT_MODULE))
		world::_register_development_components();
	world::_clear_development_reload_snapshots();
}

void development_module_unload_callback() {
	// There's no point in fixing discontinuities in component indexing,
	// because we always remove all tail components
	// which are owned by the development module.

	if (!module_name_to_component_names.contains(VOLT_DEVELOPMENT_MODULE))
		return;

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

#endif

}
