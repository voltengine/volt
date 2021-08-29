#include "ecs.hpp"

namespace volt::ecs {

template<typename T>
bool entity_manager::has_component(uint32_t eid) const {
	return has_component_index(eid, _type_to_component_index[typeid(T)]);
}

template<typename T>
T &entity_manager::get_component(uint32_t eid) {
	auto storage = static_cast<component_storage<T> *>(
			storages[_type_to_component_index[typeid(T)]].get());
	return storage->get(storage->get_cid(eid));
}

template<typename T, typename... Args>
T &entity_manager::add_component(uint32_t eid, Args &&...args) {
	uint32_t index = _type_to_component_index[typeid(T)];
	entities[eid].mask.set(index);

	auto storage = static_cast<component_storage<T> *
			>(get_or_init_storage(index));
	return storage->add(eid, std::forward<Args>(args)...);
}

template<typename T>
void entity_manager::remove_component(uint32_t eid) {
	remove_component_index(eid, _type_to_component_index[typeid(T)]);
}

}
