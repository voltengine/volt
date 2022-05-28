#include "ecs.hpp"

#include "../math/math.hpp"
#include "../error.hpp"

namespace volt::ecs::_internal {

template<typename T>
std::bitset<VOLT_MAX_COMPONENTS> make_component_filter() {
	std::bitset<VOLT_MAX_COMPONENTS> mask;
	mask.set(component_index<T>());
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
			storages.at(component_name<T>()).get());

	for (T &component : storage->components())
		func(component);
}

template<typename T, typename... Filters>
void world::each(const std::function<void(T &, Filters &...)> &func) const {
	auto storage = static_cast<_internal::component_storage<T> *>(
			storages.at(get_component_name<T>()).get());

	std::bitset<VOLT_MAX_COMPONENTS> filter =
			_internal::make_component_filter<Filters>();

	for (uint32_t cid = 0; cid < storage->components().size(); cid++) {
		uint32_t eid = storage->eid(cid);
		if (entities[eid].mask & filter == filter)
			func(storage->get(cid), get_component<Filters>(eid)...);
	}
}

template<typename T>
void world::each(const std::function<void(uint32_t, T &)> &func, util::thread_pool &pool, uint32_t least_entities_per_job) const {
	auto storage = static_cast<_internal::component_storage<T> *>(
			storages.at(component_name<T>()).get());
	auto &components = storage->components();

	uint32_t entities_per_job = math::max(math::ceil(static_cast<float>(components.size()) / pool.thread_count()), least_entities_per_job);
	if (entities_per_job == 0)
		return;
	
	uint32_t full_job_count = components.size() / entities_per_job;
	uint32_t entities_per_last_job = components.size() % entities_per_job;

	for (uint32_t i = 0; i < full_job_count + (entities_per_last_job != 0); i++) {
		uint32_t start_cid = i * entities_per_job;
		uint32_t entity_count = i == full_job_count ? entities_per_last_job : entities_per_job;

		pool.submit([&](uint32_t thread_index) {
			for (uint32_t i = 0; i < entity_count; i++)
				func(thread_index, components[start_cid + i]);
		});
	}
}

template<typename T, typename... Filters>
void world::each(const std::function<void(uint32_t, T &, Filters &...)> &func, util::thread_pool &pool, uint32_t least_entities_per_job) const {
	std::bitset<VOLT_MAX_COMPONENTS> filter =
			_internal::make_component_filter<Filters>();

	auto storage = static_cast<_internal::component_storage<T> *>(
			storages.at(component_name<T>()).get());
	auto &components = storage->components();

	uint32_t entities_per_job = math::max(math::ceil(static_cast<float>(components.size()) / pool.thread_count()), least_entities_per_job);
	if (entities_per_job == 0)
		return;

	uint32_t full_job_count = components.size() / entities_per_job;
	uint32_t entities_per_last_job = components.size() % entities_per_job;

	for (uint32_t i = 0; i < full_job_count + (entities_per_last_job != 0); i++) {
		uint32_t start_cid = i * entities_per_job;
		uint32_t entity_count = i == full_job_count ? entities_per_last_job : entities_per_job;

		pool.submit([&](uint32_t thread_index) {
			for (uint32_t i = 0; i < entity_count; i++) {
				uint32_t cid = start_cid + i;
				uint32_t eid = storage->eid(cid);

				// Kind of inefficient, because some jobs will skip more than others
				if (entities[eid].mask & filter == filter)
					func(thread_index, storage->get(cid), get_component<Filters>(eid)...);
			}
		});
	}
}

template<typename T>
bool world::has_component(uint32_t eid) const {
	return entities[eid].mask.test(
			_internal::component_index<T>());
}

template<typename T>
T &world::get_component(uint32_t eid) {
	auto storage = static_cast<_internal::component_storage<T> *>(
			storages[get_component_name<T>()].get());
	return storage->get(storage->get_cid(eid));
}

template<typename T, typename... Args>
T &world::add_component(uint32_t eid, Args &&...args) {
	uint32_t index = _internal::component_index<T>();
	entities[eid].mask.set(index);

	auto storage = static_cast<_internal::component_storage<T> *>(
			storages[get_component_name<T>()].get());
	return storage->add(eid, std::forward<Args>(args)...);
}

template<typename T>
void world::remove_component(uint32_t eid) {
	remove_component_by_name(eid, component_name<T>());
}

}
