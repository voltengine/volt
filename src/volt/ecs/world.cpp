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
	instance_it = instances.emplace(instances.end(), this);

	// Create empty archetype
	archetypes.emplace_back();
	component_index_to_archetype_indices.resize(component_count());

	// Create root entity
	entities.emplace_back();
}

world::~world() {
	instances.erase(instance_it);
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

#ifdef VOLT_DEVELOPMENT

void world::_register_development_components() {
	// For each world
	for (auto *instance : instances) {
		// Extend archetypes if necessary
		for (_internal::archetype &archetype : instance->archetypes) {
			if (archetype.mask.size() < component_count()) {
				archetype.mask.resize(component_count());
				archetype.edges.resize(component_count());
				archetype.storages.resize(component_count());
			}
		}
		instance->component_index_to_archetype_indices.resize(component_count());

		// For each development component
		for (auto &name : module_name_to_component_names[VOLT_DEVELOPMENT_MODULE]) {
			size_t index = component_name_to_index[name];

			// Recover state
			if (instance->development_reload_snapshot.contains(name)) {
				for (auto &item : instance->development_reload_snapshot[name])
					instance->add_component_by_name(item.first, name, item.second);
			}
		}
	}
}

void world::_unregister_development_components() {
	// For each world
	for (auto instance : instances) {
		// For each development component
		for (auto &name : module_name_to_component_names[VOLT_DEVELOPMENT_MODULE]) {
			size_t index = component_name_to_index[name];

			// For each archetype that contains the component
			for (size_t archetype_index : instance->component_index_to_archetype_indices[index]) {
				_internal::archetype &archetype = instance->archetypes[archetype_index];
				
				// For each entity in archetype
				auto storage = archetype.storages[index].get();
				for (size_t cid = 0; cid < archetype.size; cid++) {
					size_t eid = archetype.cid_to_eid[cid];

					// Serialize component and store it in the relaod snapshot
					instance->development_reload_snapshot[name][eid] = storage->get_json(cid);

					// Remove component from entity
					// (This also removes entity from the archetype)
					instance->remove_component_by_name(eid, name);
				}

				// Remove connections that point to this archetype
				for (size_t i = 0; i < component_count(); i++) {
					_internal::archetype::edge edge = archetype.edges[i];

					if (edge.add != std::numeric_limits<size_t>::max()) {
						_internal::archetype &connected = instance->archetypes[edge.add];
						connected.edges[i].remove = std::numeric_limits<size_t>::max();
					}

					if (edge.remove != std::numeric_limits<size_t>::max()) {
						_internal::archetype &connected = instance->archetypes[edge.remove];
						connected.edges[i].add = std::numeric_limits<size_t>::max();
					}
				}

				// If we were messing with a non-tail archetype
				if (archetype_index != instance->archetypes.size() - 1) {
					// Replace meesed up archetype with the last one in the list
					archetype = std::move(instance->archetypes.back());
					// ("archetype" now has the data of the moved, valid archetype)

					// Replace archetype indices in entities
					for (size_t cid = 0; cid < archetype.size; cid++) {
						size_t eid = archetype.cid_to_eid[cid];
						instance->entities[eid].archetype = archetype_index;
					}

					// Replace archetype indices in connections
					for (size_t i = 0; i < component_count(); i++) {
						_internal::archetype::edge edge = archetype.edges[i];

						if (edge.add != std::numeric_limits<size_t>::max()) {
							_internal::archetype &connected = instance->archetypes[edge.add];
							connected.edges[i].remove = archetype_index;
						}

						if (edge.remove != std::numeric_limits<size_t>::max()) {
							_internal::archetype &connected = instance->archetypes[edge.remove];
							connected.edges[i].add = archetype_index;
						}
					}
				}

				// Delete the messed up archetype
				instance->archetypes.pop_back();
			}

			// Remove component index from the world
			instance->component_index_to_archetype_indices[index].clear();
		}
	}
}

void world::_clear_development_reload_snapshots() {
	for (auto instance : instances)
		instance->development_reload_snapshot.clear();
}

#endif

std::list<world *> world::instances;

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
