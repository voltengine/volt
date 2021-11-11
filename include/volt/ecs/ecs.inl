#include "../modules.hpp"

#include "../error.hpp"
#include "component_storage.hpp"

namespace volt::ecs {

template<typename T>
void register_component(const std::string &name) {
	using namespace _internal;

	VOLT_ASSERT(!component_name_to_index.contains(name),
			"Component name \"" + name + "\" is already registered.")
	VOLT_ASSERT(!component_type_index_to_name.contains(typeid(T)),
			"Cannot register type as component \"" + name +
			"\". It's already registered as \"" +
			component_type_index_to_name[typeid(T)] + "\".")

	size_t index = get_component_count();
	VOLT_ASSERT(index < VOLT_MAX_COMPONENTS, "Too many components. "
			"Please bump VOLT_MAX_COMPONENTS before building.")

	component_type_index_to_name[typeid(T)] = name;

	component_name_to_index[name] = index;
	component_type_index_to_index[typeid(T)] = index;

	component_name_to_storage_constructor[name] = []() {
		return new component_storage<T>;
	};

	module_name_to_component_names[modules::this_module_name()].emplace(name);
}

template<system_type T>
void register_system(const std::string &name) {
	using namespace _internal;

	VOLT_ASSERT(!system_name_to_constructor.contains(name),
			"System name \"" + name + "\" is already registered.")

	system_type_index_to_name[typeid(T)] = name;

	system_name_to_constructor[name] = []() {
		return new T;
	};

	system_instances[name] = nullptr;

	module_name_to_system_names[modules::this_module_name()].emplace(name);
}

template<typename T>
const std::string &get_component_name() {
	using namespace _internal;

	VOLT_DEVELOPMENT_ASSERT(component_type_index_to_name.contains(typeid(T)),
			"No such component type registered: " + typeid(T).name())
	return component_type_index_to_name[typeid(T)];
}

template<typename T>
const std::string &get_system_name() {
	using namespace _internal;

	VOLT_DEVELOPMENT_ASSERT(system_type_index_to_name.contains(typeid(T)),
			"No such system type registered: " + typeid(T).name())
	return system_type_index_to_name[typeid(T)];
}

}

namespace volt::ecs::_internal {

template<typename T>
size_t get_component_index() {
	VOLT_DEVELOPMENT_ASSERT(component_type_index_to_index.contains(typeid(T)),
			"No such component type registered: " + typeid(T).name())
	return component_type_index_to_index[typeid(T)];
}

}
