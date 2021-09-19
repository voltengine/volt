#include "../modules.hpp"

#include "../error.hpp"
#include "component_storage.hpp"
#include "entity.hpp"
#include "system.hpp"
#include "world.hpp"

namespace volt::ecs {

template<typename T>
void register_component(const std::string &name) {
	VOLT_ASSERT(!_component_name_to_index.contains(name),
			"Component name \"" + name + "\" is already registered.")
	VOLT_ASSERT(!_component_type_index_to_name.contains(typeid(T)),
			"Cannot register type as component \"" + name +
			"\". It's already registered as \"" +
			_component_type_index_to_name[typeid(T)] + "\".")

	size_t index = get_component_count();
	VOLT_ASSERT(index < VOLT_MAX_COMPONENTS, "Too many components. "
			"Please bump VOLT_MAX_COMPONENTS before building.")

	_component_type_index_to_name[typeid(T)] = name;

	_component_name_to_index[name] = index;
	_component_type_index_to_index[typeid(T)] = index;

	_component_name_to_storage_constructor[name] = []() {
		return new _component_storage<T>;
	};

	_module_name_to_component_names[modules::this_module_name()].emplace(name);

	world::_register_component(name);
}

template<typename T>
const std::string &get_component_name() {
	return _component_type_index_to_name[typeid(T)];
}

template<system_type T>
void register_system(const std::string &name) {
	VOLT_ASSERT(!_system_name_to_constructor.contains(name),
			"System name \"" + name + "\" is already registered.")

	_system_name_to_constructor[name] = []() {
		return new T;
	};

	_system_instances[name] = nullptr;

	_module_name_to_system_names[modules::this_module_name()].emplace(name);
}

}
