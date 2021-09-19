#include "ecs.hpp"

#include "../error.hpp"

namespace volt::ecs {

template<typename T>
std::bitset<VOLT_MAX_COMPONENTS> _make_component_filter() {
	VOLT_DEVELOPMENT_ASSERT(_component_type_index_to_index.contains(typeid(T)),
			std::string("No such component registered: ") + typeid(T).name())

	std::bitset<VOLT_MAX_COMPONENTS> mask;
	mask.set(_component_type_index_to_index[typeid(T)]);
	return mask;
}

template<typename T, typename T2, typename... Tn>
std::bitset<VOLT_MAX_COMPONENTS> _make_component_filter() {
	return _make_component_filter<T>() | _make_component_filter<T2, Tn...>();
}

template<typename T>
void world::each(const std::function<void(T &)> &func) const {
	VOLT_DEVELOPMENT_ASSERT(_component_type_index_to_index.contains(typeid(T)),
			std::string("No such component registered: ") + typeid(T).name())

	auto storage = static_cast<_component_storage<T> *>(
			storages.at(_component_type_index_to_name[typeid(T)]).get());

	for (T &component : storage->get_components())
		func(component);
}

template<typename T, typename... Filters>
void world::each(const std::function<void(T &, Filters &...)> &func) const {
	VOLT_DEVELOPMENT_ASSERT(_component_type_index_to_index.contains(typeid(T)),
			std::string("No such component registered: ") + typeid(T).name())

	auto storage = static_cast<_component_storage<T> *>(
			storages.at(_component_type_index_to_name[typeid(T)]).get());

	std::bitset<VOLT_MAX_COMPONENTS> filter = _make_component_filter<Filters>();

	for (uint32_t cid = 0; cid < storage->get_components().size(); cid++) {
		uint32_t eid = storage->get_eid(cid);
		if (entities[eid].mask & filter == filter)
			func(storage->get(cid), get_component<Filters>(eid)...);
	}
}

template<typename T>
bool world::has_component(uint32_t eid) const {
	return entities[eid].mask.test(
			_component_type_index_to_index[typeid(T)]);
}

template<typename T>
T &world::get_component(uint32_t eid) {
	auto storage = static_cast<_component_storage<T> *>(
			storages[_component_type_index_to_name[typeid(T)]].get());
	return storage->get(storage->get_cid(eid));
}

template<typename T, typename... Args>
T &world::add_component(uint32_t eid, Args &&...args) {
	uint32_t index = _component_type_index_to_index[typeid(T)];
	entities[eid].mask.set(index);

	auto storage = static_cast<_component_storage<T> *>(
			storages[_component_type_index_to_name[typeid(T)]].get());
	return storage->add(eid, std::forward<Args>(args)...);
}

template<typename T>
void world::remove_component(uint32_t eid) {
	remove_component_by_name(eid, _component_type_index_to_name[typeid(T)]);
}

}
