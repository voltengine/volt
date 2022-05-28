#include <type_traits>

namespace volt::ecs::_internal {

template<typename T>
template<typename... Args>
T &component_storage<T>::add(Args &&...args) {
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
void component_storage<T>::add_json(const nlohmann::json &json) {
	if constexpr (std::is_convertible_v<nlohmann::json, T>)
		add(json);
	else if constexpr (std::is_default_constructible_v<T>)
		add();
	else
		VOLT_ASSERT(false, "Component must be either default constructible or JSON constructible.");
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
	components[cid] = std::move(components.back());
	components.pop_back();
}

template<typename T>
void component_storage<T>::copy_from(
		const base_component_storage &storage, size_t cid) {
	components.emplace_back(static_cast<const component_storage<T> &>(storage).components[cid]);
}

}
