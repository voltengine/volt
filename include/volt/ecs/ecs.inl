#include "component_storage.hpp"

namespace volt::ecs {

template<typename T>
const std::string &component_name() {
	using namespace _internal;

	VOLT_DEVELOPMENT_ASSERT(component_type_index_to_index.contains(typeid(T)),
			std::string("No such component type registered: ") + typeid(T).name())

	// It's not performance critical so we choose to not declare a reverse lookup map
	size_t index = component_type_index_to_index[typeid(T)];
	return std::find_if(
		component_name_to_index.begin(),
		component_name_to_index.end(),
		[&](auto &item) {
			return item.second == index;
		}
	)->first;
}

}

namespace volt::ecs::_internal {

template<typename T>
void register_component(const std::string &name) {
	VOLT_ASSERT(!component_name_to_index.contains(name),
			"Component name \"" + name + "\" is already registered.")
	VOLT_ASSERT(!component_type_index_to_index.contains(typeid(T)),
			"Cannot register type as component \"" + name +
			"\". It's already registered as \"" +
			std::find_if(
				component_name_to_index.begin(),
				component_name_to_index.end(),
				[](auto &item) {
					return item.second == component_type_index_to_index[typeid(T)];
				}
			)->first + "\".")

	size_t component_index = component_type_index_to_index.size();

	module_name_to_component_names[modules::this_module_name()].push_back(name);

	component_type_index_to_index.emplace(typeid(T), component_index);
	component_name_to_index.emplace(name, component_index);

	component_index_to_storage_constructor.emplace_back([]() {
		return new component_storage<T>;
	});
}

}
