#include <volt/ecs/component_storage.hpp>

namespace volt::ecs {

size_t base_component_storage::get_cid(size_t eid) const {
	return eid_to_cid[eid];
}

size_t base_component_storage::get_eid(size_t cid) const {
	return cid_to_eid[cid];
}

}
