#include "ecs.hpp"

#include "../error.hpp"

namespace volt::ecs::_internal {

template<typename T>
std::bitset<VOLT_MAX_COMPONENTS> make_component_filter() {
	std::bitset<VOLT_MAX_COMPONENTS> mask;
	mask.set(get_component_index<T>());
	return mask;
}

template<typename T, typename T2, typename... Tn>
std::bitset<VOLT_MAX_COMPONENTS> make_component_filter() {
	return make_component_filter<T>() | make_component_filter<T2, Tn...>();
}

}

namespace volt::ecs {

template<typename T>
void world::each(const std::function<void(T &)> &func) const {
	auto storage = static_cast<_internal::component_storage<T> *>(
			storages.at(get_component_name<T>()).get());

	for (T &component : storage->get_components())
		func(component);
}

template<typename T, typename... Filters>
void world::each(const std::function<void(T &, Filters &...)> &func) const {
	auto storage = static_cast<_internal::component_storage<T> *>(
			storages.at(get_component_name<T>()).get());

	std::bitset<VOLT_MAX_COMPONENTS> filter =
			_internal::make_component_filter<Filters>();

	for (uint32_t cid = 0; cid < storage->get_components().size(); cid++) {
		uint32_t eid = storage->get_eid(cid);
		if (entities[eid].mask & filter == filter)
			func(storage->get(cid), get_component<Filters>(eid)...);
	}
}

template<typename T>
bool world::has_component(uint32_t eid) const {
	return entities[eid].mask.test(
			_internal::get_component_index<T>());
}

template<typename T>
T &world::get_component(uint32_t eid) {
	auto storage = static_cast<_internal::component_storage<T> *>(
			storages[get_component_name<T>()].get());
	return storage->get(storage->get_cid(eid));
}

template<typename T, typename... Args>
T &world::add_component(uint32_t eid, Args &&...args) {
	uint32_t index = _internal::get_component_index<T>();
	entities[eid].mask.set(index);

	auto storage = static_cast<_internal::component_storage<T> *>(
			storages[get_component_name<T>()].get());
	return storage->add(eid, std::forward<Args>(args)...);
}

template<typename T>
void world::remove_component(uint32_t eid) {
	remove_component_by_name(eid, get_component_name<T>());
}

}
