#include "../modules/modules.hpp"

#include "component_storage.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"

namespace volt::ecs {

template<typename T>
void register_component(const std::string &name) {
	bool already_registered = _component_name_to_index.contains(name);
	size_t index = already_registered
			? _component_name_to_index[name]
			: _component_index_counter++;

	modules::register_serializable<component_storage<
			T>>(_make_component_storage_name(name));
	
	if (already_registered) {
		std::type_index i = _component_to_type_index[index];
		_type_to_component_index.erase(i);
		_component_to_type_index[index] = typeid(T);

		// This must happen after erase or else an
		// invalid type_index would be compared
		_type_to_component_index[typeid(T)] = index;
		return;
	} else {
		_component_to_type_index.emplace_back(typeid(T));
		_type_to_component_index[typeid(T)] = index;
	}
	


	if (index >= VOLT_MAX_COMPONENTS) {
		throw std::runtime_error("Too many components. "
				"Please bump VOLT_MAX_COMPONENTS before building.");
	}

	_component_index_to_name.emplace_back(name);
	_component_name_to_index[name] = index;
}

template<typename T>
const std::string &get_component_name() {
	return _component_index_to_name[_type_to_component_index[typeid(T)]];
}

}
