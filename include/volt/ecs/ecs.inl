#include "component_storage.hpp"

namespace volt::ecs {

template<typename T>
const std::string &component_name() {
	using namespace _internal;

	VOLT_DEVELOPMENT_ASSERT(component_type_index_to_index.contains(typeid(T)),
			std::string("No such component type registered: ") + typeid(T).name())

	// It's not performance critical so we choose to not declare a reverse lookup map
	size_t index = component_type_index_to_index[typeid(T)];
	for (auto &item : component_name_to_index) {
		if (item.second == index)
			return item.first;
	}
}

}

namespace volt::ecs::_internal {

template<typename T>
void register_component(const std::string &name) {
	size_t component_index = component_type_index_to_index.size();

	component_type_index_to_index.emplace(typeid(T), component_index);
	component_name_to_index.emplace(name, component_index);

	component_index_to_size.emplace_back(sizeof(T));
	component_index_to_storage_constructor.emplace_back([]() {
		return new component_storage<T>;
	});
}

}
