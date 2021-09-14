#include <volt/pch.hpp>
#include <volt/ecs/world.hpp>

#include <algorithm>

#include <volt/ecs/entity.hpp>

namespace volt::ecs {

world::world() {
	instance_it = instances.emplace(instances.end(), this);

	for (auto &item : _component_name_to_storage_constructor)
		storages[item.first].reset(item.second());
}

world::~world() {
	instances.erase(instance_it);
}

entity world::create() {
	uint32_t eid;

	if (free_eids.empty()) {
		eid = entities.size();
		entities.emplace_back();
	} else {
		eid = free_eids.back();
		free_eids.pop_back();
	}

	return entity(this, eid, entities[eid].version);
}

void world::_register_component(const std::string &name) {
	for (auto instance : instances) {
		// Init storage
		_base_component_storage *storage = _component_name_to_storage_constructor[name]();
		instance->storages[name].reset(storage);

		// Recover state
		if (instance->state_snapshots.contains(name)) {
			for (auto &item : instance->state_snapshots[name])
				instance->add_component_by_name(item.first, name, item.second);
			
			instance->state_snapshots.erase(name);
		}
	}
}

void world::_unregister_component(const std::string &name, bool remember_state, const std::string &module_name) {
	size_t index = _component_name_to_index[name];
	size_t last_index = get_component_count() - 1;

	for (auto instance : instances) {
		// Save state
		if (remember_state) {
			for (uint32_t eid = 0; eid < instance->entities.size(); eid++) {
				if (std::find(instance->free_eids.begin(),
						instance->free_eids.end(), eid)
						!= instance->free_eids.end())
					continue;

				if (instance->entities[eid].mask.test(index)) {
					auto storage = instance->storages[name].get();
					instance->state_snapshots[name][eid] =
							storage->get_json(storage->get_cid(eid));
				}
			}
			instance->module_name_to_state_snapshot_name[module_name] = name;
		}

		// Remove storage and optimize component indices
		instance->storages.erase(name);
		for (entity_data &data : instance->entities) {
			data.mask[index] = data.mask[last_index];
			data.mask.reset(last_index);
		}
	}
}

void world::_clear_state_snapshot(const std::string &module_name) {
	for (auto instance : instances) {
		if (!instance->module_name_to_state_snapshot_name
				.contains(module_name))
			continue;
		
		instance->state_snapshots.erase(instance->
				module_name_to_state_snapshot_name[module_name]);
		instance->module_name_to_state_snapshot_name.erase(module_name);
	}
}

std::list<world *> world::instances;

bool world::expired(uint32_t eid, uint32_t version) const {
	return entities[eid].version != version;
}

void world::destroy(uint32_t eid) {
	entity_data &data = entities[eid];

	for (auto &item : _component_name_to_index) {
		if (data.mask.test(item.second)) {
			auto &storage = storages[item.first];
			storage->remove(storage->get_cid(eid));
		}
	}

	data.mask.reset();
	data.version++;

	free_eids.push_back(eid);
}

bool world::has_component_by_name(
		uint32_t eid, const std::string &name) const {
	return entities[eid].mask.test(_component_name_to_index[name]);
}

void world::add_component_by_name(uint32_t eid,
		const std::string &name, const nlohmann::json &json) {
	entities[eid].mask.set(_component_name_to_index[name]);
	storages[name]->add_json(eid, json);
}

nlohmann::json world::get_component_by_name(
		uint32_t eid, const std::string &name) {
	auto storage = storages[name].get();
	return storage->get_json(storage->get_cid(eid));
}

void world::remove_component_by_name(
		uint32_t eid, const std::string &name) {
	entities[eid].mask.reset(_component_name_to_index[name]);
	auto storage = storages[name].get();
	storage->remove(storage->get_cid(eid));
}

volt::ecs::world global_test_world; // TODO: Remove

}
