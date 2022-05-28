#include <volt/pch.hpp>
#include <volt/ecs/world.hpp>

#include <algorithm>

#include <volt/ecs/entity.hpp>

namespace volt::ecs::_internal {

archetype::archetype() {
	mask.resize(component_count());
	edges.resize(component_count());
	storages.resize(component_count());
}

archetype::archetype(const util::dynamic_bitset &mask) : mask(mask) {
	edges.resize(component_count());
	storages.resize(component_count());
}

}

namespace volt::ecs {

using namespace _internal;

world::world() {
	// Create empty archetype
	archetypes.emplace_back();
	component_index_to_archetype_indices.resize(component_count());

	// Create root entity
	entities.emplace_back();
}

world::~world() {
	
}

entity world::create() {
	size_t eid;

	if (free_eids.empty()) {
		eid = entities.size();
		entities.emplace_back();
	} else {
		eid = free_eids.back();
		free_eids.pop_back();
	}

	entity_data &data = entities[eid];

	// Add to empty archetype
	data.archetype = 0;
	add_eid_cid_mapping_to_archetype(archetypes.front(), eid);

	return entity(this, eid, data.version);
}

entity world::root() {
	// Root entity can never be destroyed, so its version is always zero
	return entity(this, 0, 0);
}

bool world::expired(size_t eid, size_t version) const {
	return entities[eid].version != version;
}

void world::destroy(size_t eid) {
	VOLT_DEVELOPMENT_ASSERT(eid != 0, "Attempted to destroy root entity.");

	entity_data &data = entities[eid];
	archetype &archetype = archetypes[data.archetype];

	remove_eid_cid_mapping_from_archetype(archetype, eid);
	for (size_t cid = 0; cid < component_count(); cid++) {
		if (archetype.mask.test(cid))
			archetype.storages[cid]->remove(eid);
	}

	data.version++;

	free_eids.push_back(eid);
}

bool world::has_component_by_name(
		size_t eid, const std::string &name) const {
	const _internal::archetype &archetype = archetypes[entities[eid].archetype];
	return archetype.mask.test(component_name_to_index[name]);
}

nlohmann::json world::get_component_by_name(
		size_t eid, const std::string &name) const {
	const _internal::archetype &archetype = archetypes[entities[eid].archetype];
	const base_component_storage &storage = *archetype.storages[component_name_to_index[name]].get();
	return storage.get_json(archetype.eid_to_cid[eid]);
}

void world::add_component_by_name(size_t eid,
		const std::string &name, const nlohmann::json &json) {
	size_t component_index = _internal::component_name_to_index[name];
	size_t archetype_index = entities[eid].archetype;
	size_t new_archetype_index = add_component_to_archetype(archetypes,
			component_index_to_archetype_indices, archetype_index, component_index);

	entities[eid].archetype = new_archetype_index;
	_internal::move_entity_to_archetype(archetypes[archetype_index], archetypes[new_archetype_index], eid);
	
	archetypes[new_archetype_index].storages[component_index]->add_json(json);
}

void world::remove_component_by_name(
		size_t eid, const std::string &name) {
	size_t component_index = _internal::component_name_to_index[name];
	size_t archetype_index = entities[eid].archetype;
	size_t new_archetype_index = remove_component_from_archetype(archetypes,
			component_index_to_archetype_indices, archetype_index, component_index);

	entities[eid].archetype = new_archetype_index;
	_internal::move_entity_to_archetype(archetypes[archetype_index], archetypes[new_archetype_index], eid);
}

}
