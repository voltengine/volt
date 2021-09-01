#include <volt/pch.hpp>
#include <volt/ecs/entity_manager.hpp>

#include <volt/ecs/entity.hpp>

namespace volt::ecs {

entity_manager::entity_manager() {
	instance_it = _instances.emplace(_instances.end(), this);
}

entity_manager::~entity_manager() {
	_instances.erase(instance_it);
}

entity entity_manager::create() {
	uint32_t eid;

	if (free_eids.empty()) {
		eid = _entities.size();
		_entities.emplace_back();
	} else {
		eid = free_eids.back();
		free_eids.pop_back();
	}

	return entity(this, eid, _entities[eid].version);
}

std::list<entity_manager *> entity_manager::_instances;

bool entity_manager::expired(uint32_t eid, uint32_t version) const {
	return _entities[eid].version != version;
}

void entity_manager::destroy(uint32_t eid) {
	_entity_data &data = _entities[eid];

	for (size_t i = 0; i < VOLT_MAX_COMPONENTS; i++) {
		if (data.mask.test(i)) {
			auto &storage = _storages[i];
			storage->remove(storage->get_cid(eid));
		}
	}

	data.mask.reset();
	data.version++;

	free_eids.push_back(eid);
}

bool entity_manager::has_component_index(
		uint32_t eid, size_t index) const {
	return _entities[eid].mask.test(index);
}

void entity_manager::add_component_index(uint32_t eid,
		size_t index, const nlohmann::json &json) {
	_entities[eid].mask.set(index);
	get_or_init_storage(index)->add_json(eid, json);
}

nlohmann::json entity_manager::get_component_index(
		uint32_t eid, size_t index) {
	auto storage = _storages[index].get();
	return storage->get_json(storage->get_cid(eid));
}

void entity_manager::remove_component_index(uint32_t eid, size_t index) {
	_entities[eid].mask.reset(index);
	auto storage = _storages[index].get();
	storage->remove(storage->get_cid(eid));
}

_base_component_storage *entity_manager::get_or_init_storage(size_t index) {
	if (!_storages[index]) {
		return (
			_storages[index] = modules::instantiate_serializable(
			_make_component_storage_name(_component_index_to_name[index]))
		).get();
	} else
		return _storages[index].get();
}

}
