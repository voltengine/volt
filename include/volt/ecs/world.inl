#include <concepts>

#include "../math/math.hpp"
#include "../error.hpp"
#include "ecs.hpp"
#include "entity.hpp"

namespace volt::ecs::_internal {

template<typename T>
void build_component_filter(util::dynamic_bitset &filter) {
	filter.set(component_type_index_to_index[typeid(T)]);
}

template<typename T1, typename T2, typename... Tn>
void build_component_filter(util::dynamic_bitset &filter) {
	build_component_filter<T1>(filter);
	build_component_filter<T2, Tn...>(filter);
}

template<typename T>
component_storage<T> &access_storage(_internal::archetype &archetype) {
	return *static_cast<component_storage<T> *>(archetype.storages[
			component_type_index_to_index[typeid(T)]].get());
}

template<typename... Components, typename... Storages>
void iterate_each_storage(
		const _internal::archetype &archetype,
		const std::function<void(Components &...)> &func,
		Storages &...storages) {
	for (size_t cid = 0; cid < archetype.size; cid++)
		func(storages.components[cid]...);
}

template<typename... Components, typename... Storages>
void iterate_each_storage(
	ecs::world *world,
	const _internal::archetype &archetype,
	const std::function<void(entity, Components &...)> &func,
	Storages &...storages) {
	for (size_t cid = 0; cid < archetype.size; cid++) {
		size_t eid = archetype.cid_to_eid[cid];
		entity handle = entity::_make(world, eid);

		func(handle, storages.components[cid]...);
	}
}

static size_t remove_eid_cid_mapping_from_archetype(
		_internal::archetype &archetype, size_t eid) {
	size_t old_cid = archetype.eid_to_cid[eid];

	size_t back_cid = archetype.size - 1;
	size_t back_eid = archetype.cid_to_eid[back_cid];

	archetype.cid_to_eid[old_cid] = back_eid;
	archetype.eid_to_cid[back_eid] = old_cid;

	archetype.size--;
	return old_cid;
}

static size_t add_eid_cid_mapping_to_archetype(
		_internal::archetype &archetype, size_t eid) {
	size_t new_cid = archetype.size;

	if (eid >= archetype.eid_to_cid.size())
		archetype.eid_to_cid.resize(eid + 1);
	if (new_cid >= archetype.cid_to_eid.size())
		archetype.cid_to_eid.resize(new_cid + 1);

	archetype.eid_to_cid[eid] = new_cid;
	archetype.cid_to_eid[new_cid] = eid;

	archetype.size++;
	return new_cid;
}

static void move_entity_to_archetype(archetype &from,
		archetype &to, size_t eid) {
	// Remove old eid <-> cid mapping
	size_t old_cid = remove_eid_cid_mapping_from_archetype(from, eid);

	// Add new eid <-> cid mapping
	size_t new_cid = add_eid_cid_mapping_to_archetype(to, eid);

	// Copy components to new storages
	for (size_t i = 0; i < _internal::component_count(); i++) {
		if (from.mask.test(i)) {
			if (to.mask.test(i))
				to.storages[i]->copy_from(*from.storages[i].get(), old_cid);

			from.storages[i]->remove(old_cid);
		}
	}
}

static size_t add_component_to_archetype(
		std::vector<_internal::archetype> &archetypes,
		std::vector<std::vector<size_t>> &component_index_to_archetype_indices,
		size_t old_archetype_index, size_t component_index) {
	archetype &old_archetype = archetypes[old_archetype_index];
	size_t &new_archetype_index = old_archetype.edges[component_index].add;

	// If there is no connection in the graph
	if (new_archetype_index == std::numeric_limits<size_t>::max()) {
		util::dynamic_bitset mask = old_archetype.mask;
		mask.set(component_index);

		// Try to find the archetype
		for (size_t i = 0; i < archetypes.size(); i++) {
			if (archetypes[i].mask == mask) {
				new_archetype_index = i; // Connect A
				archetypes[i].edges[component_index].remove = old_archetype_index; // Connect B
				return i; // Eventually terminating here...
			}
		}

		// But if there's still no match then create a new archetype
		new_archetype_index = archetypes.size(); // Connect A
		archetype &new_archetype = archetypes.emplace_back(std::move(mask));

		new_archetype.mask.set(component_index);
		new_archetype.edges[component_index].remove = old_archetype_index; // Connect B

		for (size_t i = 0; i < component_count(); i++) {
			if (new_archetype.mask.test(i)) {
				new_archetype.storages[i].reset(component_index_to_storage_constructor[i]());
				component_index_to_archetype_indices[i].emplace_back(new_archetype_index);
			}
		}
	}

	return new_archetype_index;
}

static size_t remove_component_from_archetype(
		std::vector<_internal::archetype> &archetypes,
		std::vector<std::vector<size_t>> &component_index_to_archetype_indices,
		size_t old_archetype_index, size_t component_index) {
	archetype &old_archetype = archetypes[old_archetype_index];
	size_t &new_archetype_index = old_archetype.edges[component_index].remove;

	// If there is no connection in the graph
	if (new_archetype_index == std::numeric_limits<size_t>::max()) {
		util::dynamic_bitset mask = old_archetype.mask;
		mask.reset(component_index);

		// Try to find the archetype
		for (size_t i = 0; i < archetypes.size(); i++) {
			if (archetypes[i].mask == mask) {
				new_archetype_index = i; // Connect A
				archetypes[i].edges[component_index].add = old_archetype_index; // Connect B
				return i; // Eventually terminating here...
			}
		}

		// But if there's still no match then create a new archetype
		new_archetype_index = archetypes.size(); // Connect A
		archetype &new_archetype = archetypes.emplace_back(std::move(mask));

		new_archetype.mask.set(component_index);
		new_archetype.edges[component_index].add = old_archetype_index; // Connect B

		for (size_t i = 0; i < component_count(); i++) {
			if (new_archetype.mask.test(i)) {
				new_archetype.storages[i].reset(component_index_to_storage_constructor[i]());
				component_index_to_archetype_indices[i].emplace_back(new_archetype_index);
			}
		}
	}

	return new_archetype_index;
}

}

namespace volt::ecs {

template<typename T>
void world::each(const std::type_identity_t<std::function<void(T &)>> &func) {
	size_t index = _internal::component_type_index_to_index[typeid(T)];

	for (size_t archetype_index : component_index_to_archetype_indices[index]) {
		_internal::archetype &archetype = archetypes[archetype_index];
		if (archetype.mask.test(index)) {
			iterate_each_storage(archetype, func,
					access_storage<T>(archetype));
		}
	}
}

template<typename T>
void world::each(const std::type_identity_t<std::function<void(entity, T &)>> &func) {
	size_t index = _internal::component_type_index_to_index[typeid(T)];

	for (size_t archetype_index : component_index_to_archetype_indices[index]) {
		_internal::archetype &archetype = archetypes[archetype_index];
		if (archetype.mask.test(index)) {
			iterate_each_storage(this, archetype, func,
					access_storage<T>(archetype));
		}
	}
}

template<typename T1, typename T2, typename... Tn>
void world::each(const std::type_identity_t<std::function<
		void(T1 &, T2 &, Tn &...)>> &func) {
	size_t index = _internal::component_type_index_to_index[typeid(T1)];

	util::dynamic_bitset filter(_internal::component_count());
	_internal::build_component_filter<T1, T2, Tn...>(filter);

	for (size_t archetype_index : component_index_to_archetype_indices[index]) {
		_internal::archetype &archetype = archetypes[archetype_index];
		if ((archetype.mask & filter) == filter) {
			iterate_each_storage(archetype, func,
					access_storage<T1>(archetype),
					access_storage<T2>(archetype),
					access_storage<Tn>(archetype)...);
		}
	}
}

template<typename T1, typename T2, typename... Tn>
void world::each(const std::type_identity_t<std::function<
		void(entity, T1 &, T2 &, Tn &...)>> &func) {
	size_t index = _internal::component_type_index_to_index[typeid(T1)];

	util::dynamic_bitset filter(_internal::component_count());
	_internal::build_component_filter<T1, T2, Tn...>(filter);

	for (size_t archetype_index : component_index_to_archetype_indices[index]) {
		_internal::archetype &archetype = archetypes[archetype_index];
		if ((archetype.mask & filter) == filter) {
			iterate_each_storage(this, archetype, func,
					access_storage<T1>(archetype),
					access_storage<T2>(archetype),
					access_storage<Tn>(archetype)...);
		}
	}
}

template<typename T>
bool world::has_component(size_t eid) const {
	const _internal::archetype &archetype = archetypes[entities[eid].archetype];
	return archetype.mask.test(_internal::component_type_index_to_index[typeid(T)]);
}

template<typename T>
T &world::get_component(size_t eid) {
	_internal::archetype &archetype = archetypes[entities[eid].archetype];
	return access_storage<T>(archetype).components[archetype.eid_to_cid[eid]];
}

template<typename T, typename... Args>
T &world::add_component(size_t eid, Args &&...args) {
	size_t component_index = _internal::component_type_index_to_index[typeid(T)];
	size_t archetype_index = entities[eid].archetype;
	size_t new_archetype_index = add_component_to_archetype(archetypes,
			component_index_to_archetype_indices, archetype_index, component_index);

	entities[eid].archetype = new_archetype_index;
	_internal::move_entity_to_archetype(archetypes[archetype_index], archetypes[new_archetype_index], eid);

	return _internal::access_storage<T>(archetypes[new_archetype_index]).add(std::forward<Args>(args)...);
}

template<typename T>
void world::remove_component(size_t eid) {
	size_t component_index = _internal::component_type_index_to_index[typeid(T)];
	size_t archetype_index = entities[eid].archetype;
	size_t new_archetype_index = remove_component_from_archetype(archetypes,
			component_index_to_archetype_indices, archetype_index, component_index);

	entities[eid].archetype = new_archetype_index;
	_internal::move_entity_to_archetype(archetypes[archetype_index], archetypes[new_archetype_index], eid);
}

}
