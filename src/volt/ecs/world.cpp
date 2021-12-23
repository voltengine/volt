#include <volt/pch.hpp>
#include <volt/ecs/world.hpp>

#include <algorithm>

#include <volt/ecs/entity.hpp>

namespace volt::ecs {

using namespace _internal;

world::world() {
	instance_it = instances.emplace(instances.end(), this);

	for (auto &item : component_name_to_storage_constructor)
		storages[item.first].reset(item.second());

	// Create root entity
	entities.emplace_back();
}

world::~world() {
	instances.erase(instance_it);
}

// entity world::create() {
// 	uint32_t eid;

// 	if (free_eids.empty()) {
// 		eid = entities.size();
// 		entities.emplace_back();
// 	} else {
// 		eid = free_eids.back();
// 		free_eids.pop_back();
// 	}

// 	return entity(this, eid, entities[eid].version);
// }

entity world::root_entity() {
	// Root entity can never be destroyed, so its version is always zero
	return entity(this, 0, 0);
}

#ifdef VOLT_DEVELOPMENT

void world::_register_development_component(const std::string &name) {
	for (auto instance : instances) {
		// Init storage
		base_component_storage *storage = component_name_to_storage_constructor[name]();
		instance->storages[name].reset(storage);

		// Recover state
		if (instance->development_reload_snapshot.contains(name)) {
			for (auto &item : instance->development_reload_snapshot[name])
				instance->add_component_by_name(item.first, name, item.second);
		}
	}
}

void world::_unregister_development_component(const std::string &name) {
	size_t index = component_index(name);
	size_t last_index = component_count() - 1;

	for (auto instance : instances) {
		// Save state
		for (uint32_t eid = 0; eid < instance->entities.size(); eid++) {
			// Find out whether eid is just leftovers after destroyed entity
			if (std::find(instance->free_eids.begin(),
					instance->free_eids.end(), eid)
					!= instance->free_eids.end())
				continue;

			if (instance->entities[eid].mask.test(index)) {
				auto storage = instance->storages[name].get();
				instance->development_reload_snapshot[name][eid] =
						storage->get_json(storage->cid(eid));
			}
		}

		// Remove storage and optimize component indices
		instance->storages.erase(name);
		for (entity_data &data : instance->entities) {
			data.mask[index] = data.mask[last_index];
			data.mask.reset(last_index);
		}
	}
}

void world::_clear_development_reload_snapshots() {
	for (auto instance : instances)
		instance->development_reload_snapshot.clear();
}

#endif

std::list<world *> world::instances;

bool world::expired(uint32_t eid, uint32_t version) const {
	return entities[eid].version != version;
}

void world::destroy(uint32_t eid) {
	VOLT_DEVELOPMENT_ASSERT(eid != 0, "Attempted to destroy root entity.");

	entity_data &data = entities[eid];

	for (auto &item : component_name_to_index) {
		if (data.mask.test(item.second)) {
			auto &storage = storages[item.first];
			storage->remove(storage->cid(eid));
		}
	}

	data.mask.reset();
	data.version++;

	free_eids.push_back(eid);
}

bool world::has_component_by_name(
		uint32_t eid, const std::string &name) const {
	return entities[eid].mask.test(component_index(name));
}

void world::add_component_by_name(uint32_t eid,
		const std::string &name, const nlohmann::json &json) {
	entities[eid].mask.set(component_index(name));
	storages[name]->add_json(eid, json);
}

nlohmann::json world::get_component_by_name(
		uint32_t eid, const std::string &name) {
	auto storage = storages[name].get();
	return storage->get_json(storage->cid(eid));
}

void world::remove_component_by_name(
		uint32_t eid, const std::string &name) {
	entities[eid].mask.reset(component_index(name));
	auto storage = storages[name].get();
	storage->remove(storage->cid(eid));
}

}
