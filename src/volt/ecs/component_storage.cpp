#include <volt/ecs/component_storage.hpp>

namespace nl = nlohmann;

namespace volt::ecs::_internal {

uint32_t base_component_storage::get_cid(uint32_t eid) const {
	return eid_to_cid[eid];
}

uint32_t base_component_storage::get_eid(uint32_t cid) const {
	return cid_to_eid[cid];
}

}
