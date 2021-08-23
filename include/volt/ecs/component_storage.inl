namespace volt::ecs {

template<component_type T>
template<typename... Args>
T &component_storage<T>::add(size_t eid, Args &&...args) {
	size_t new_cid = components.size();

	if (eid >= eid_to_cid.size())
		eid_to_cid.resize(eid + 1);
	if (new_cid >= cid_to_eid.size())
		cid_to_eid.resize(new_cid + 1);

	eid_to_cid[eid] = new_cid;
    cid_to_eid[new_cid] = eid;
	
	return components.emplace_back(std::forward<Args>(args)...);
}

template<component_type T>
T &component_storage<T>::get(size_t cid) {
	return components[id];
}

template<component_type T>
const std::vector<T> &component_storage<T>::get_components() const {
	return components;
}

template<component_type T>
void component_storage<T>::remove(size_t cid) {
	size_t back_cid = components.size() - 1;
	size_t back_eid = cid_to_eid[back_cid];

	cid_to_eid[cid] = back_eid;
	eid_to_cid[back_eid] = cid;
	
	components[cid] = std::move(components.back());
	components.pop_back();
}

template<component_type T>
const serializable *component_storage<T>
		::get_ptr(size_t cid) const {
	return &components[cid];
}

}
