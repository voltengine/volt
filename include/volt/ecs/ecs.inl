#include "../assert.hpp"
#include "../modules.hpp"
#include "component_storage.hpp"

namespace volt::ecs {

template<component_type T>
void register_component(const std::string &name) {
	if (!_added_module_reload_callback) {
		modules::add_reload_callback(_module_reload_callback);
		_added_module_reload_callback = true;
	}

	size_t index = get_component_index();
	VOLT_ASSERT(index < VOLT_MAX_COMPONENTS,
			"Too many components. Please bump VOLT_MAX_COMPONENTS before building.")

	_storage_constructors.push_back([]() {
		return std::unique_ptr<base_component_storage>(
				new component_storage<T>());
	});

	_component_index_to_name.push_back(name);
	_component_name_to_index[name] = index;
	_module_to_component_index[modules::this_module()] = index;
}

template<component_type T>
size_t get_component_index() {
	static size_t index = _component_index_counter++;
	return index;
}

}
