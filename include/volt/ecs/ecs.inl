#include "component_storage.hpp"

namespace volt::ecs::_internal {

template<typename T>
void register_component(const std::string &module_name, const std::string &name) {
	VOLT_ASSERT(!component_name_to_index.contains(name),
			"Component name \"" + name + "\" is already registered.")
	VOLT_ASSERT(!component_type_index_to_index.contains(typeid(T)),
			"Cannot register type as component \"" + name +
			"\". It's already registered as \"" + component_name(
			component_type_index_to_index[typeid(T)]) + "\".")

	size_t component_index = component_type_index_to_index.size();

	module_name_to_component_names[module_name].push_back(name);

	component_type_index_to_index.emplace(typeid(T), component_index);
	component_name_to_index.emplace(name, component_index);

	component_index_to_storage_constructor.emplace_back([]() {
		return new component_storage<T>;
	});
}

template<typename T>
void register_system(const std::string &module_name, const std::string &name) {
	VOLT_ASSERT(!system_name_to_index.contains(name),
			"system name \"" + name + "\" is already registered.")
	VOLT_ASSERT(!system_type_index_to_index.contains(typeid(T)),
			"Cannot register type as system \"" + name +
			"\". It's already registered as \"" + system_name(
			system_type_index_to_index[typeid(T)]) + "\".")

	size_t system_index = system_type_index_to_index.size();

	module_name_to_system_names[module_name].push_back(name);

	system_type_index_to_index.emplace(typeid(T), system_index);
	system_name_to_index.emplace(name, system_index);

	system_index_to_constructor.emplace_back([]() {
		return new T;
	});
}

}
