#include <type_traits>

namespace volt::ecs::_internal {

template<typename T>
template<typename... Args>
T &component_storage<T>::add(uint32_t eid, Args &&...args) {
	uint32_t new_cid = components.size();

	if (eid >= eid_to_cid.size())
		eid_to_cid.resize(eid + 1);
	if (new_cid >= cid_to_eid.size())
		cid_to_eid.resize(new_cid + 1);

	eid_to_cid[eid] = new_cid;
    cid_to_eid[new_cid] = eid;
	
	return components.emplace_back(std::forward<Args>(args)...);
}

template<typename T>
T &component_storage<T>::get(uint32_t cid) {
	return components[cid];
}

template<typename T>
const T &component_storage<T>::get(uint32_t cid) const {
	return components[cid];
}

template<typename T>
std::vector<T> &component_storage<T>::get_components() {
	return components;
}

template<typename T>
void component_storage<T>::add_json(uint32_t eid, const nlohmann::json &json) {
	if constexpr (std::is_convertible_v<nlohmann::json, T>)
		add(eid, json);
	else
		add(eid);
}

template<typename T>
nlohmann::json component_storage<T>::get_json(uint32_t cid) const {
	if constexpr (std::is_convertible_v<nlohmann::json, T>)
		return get(cid);
	else
		return nlohmann::json();
}

template<typename T>
void component_storage<T>::remove(uint32_t cid) {
	uint32_t back_cid = components.size() - 1;
	uint32_t back_eid = cid_to_eid[back_cid];

	cid_to_eid[cid] = back_eid;
	eid_to_cid[back_eid] = cid;
	
	components[cid] = std::move(components.back());
	components.pop_back();
}

}
