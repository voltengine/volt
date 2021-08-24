#include "../assert.hpp"

namespace volt::ecs {

template<typename T>
void register_component(const std::string &name) {
	bool already_registered = _component_name_to_index.contains(name);
	size_t index = already_registered
			? _component_name_to_index[name]
			: _component_index_counter++;
	VOLT_ASSERT(index < VOLT_MAX_COMPONENTS,
			"Too many components. Please bump VOLT_MAX_COMPONENTS before building.")

	if (already_registered) {
		_storage_constructors[index] = []() {
			return new component_storage<T>();
		};
	} else {
		_storage_constructors.emplace_back([]() {
			return new component_storage<T>();
		});

		if (!_added_module_reload_callback) {
			modules::add_reload_callback(_module_reload_callback);
			_added_module_reload_callback = true;
		}

		_component_index_to_name.emplace_back(name);
		_component_name_to_index[name] = index;
		_module_name_to_component_index[modules::this_module()] = index;
		_storage_owner_lists.emplace_back();
	}	
}

}
