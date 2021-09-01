#include "../modules/modules.hpp"

#include "component_storage.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

namespace volt::ecs {

template<typename T>
void register_component(const std::string &name) {
	if (_type_to_component_index.contains(typeid(T)))
		throw std::runtime_error("Component \"" + name + "\" is already registered.");

	size_t index = _component_to_type_index.size();
	if (index >= VOLT_MAX_COMPONENTS) {
		throw std::runtime_error("Too many components. "
				"Please bump VOLT_MAX_COMPONENTS before building.");
	}

	modules::register_serializable<_component_storage<
			T>>(_make_component_storage_name(name));
	
	_component_to_type_index.emplace_back(typeid(T));
	_type_to_component_index[typeid(T)] = index;

	_component_index_to_name.emplace_back(name);
	_component_name_to_index[name] = index;
}

template<typename T>
const std::string &get_component_name() {
	return _component_index_to_name[_type_to_component_index[typeid(T)]];
}

}
