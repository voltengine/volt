#include <volt/ecs/component_storage.hpp>

namespace nl = nlohmann;

namespace volt::ecs {

_base_component_storage::~_base_component_storage() {
	for (auto &entity : manager->_entities) {
		entity.mask[index] = entity.mask[last_index];
		entity.mask.reset(last_index);
	}
}

uint32_t _base_component_storage::get_cid(uint32_t eid) const {
	return eid_to_cid[eid];
}

uint32_t _base_component_storage::get_eid(uint32_t cid) const {
	return cid_to_eid[cid];
}

nlohmann::json _base_component_storage::serialize() const {
	auto json = serializable::serialize();
	json["entities"] = cid_to_eid;
	return json;
}

void _base_component_storage::deserialize(const nlohmann::json &json) {
	auto &entities = json["entities"];

	cid_to_eid.reserve(entities.size());
	eid_to_cid.resize(entities.size());

	uint32_t i = 0;
	for (auto &item : entities) {
		cid_to_eid.push_back(item);
		eid_to_cid[item] = i;
	}
	
	serializable::deserialize(json);
}

}
