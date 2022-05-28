#include "../error.hpp"
#include "assets.hpp"

namespace volt::assets {

template<typename T>
asset_ref<T>::asset_ref(_internal::asset_owner *owner) : owner(owner) {
	construct();
}

template<typename T>
template<typename U>
asset_ref<T>::asset_ref(const asset_ref<U> &other) noexcept : asset_ref(other.owner) {}

template<typename T>
template<typename U>
asset_ref<T>::asset_ref(asset_ref<U> &&other) noexcept : owner(other.owner) {
	other.owner = nullptr;
}

template<typename T>
asset_ref<T>::~asset_ref() noexcept {
	destruct();
}

template<typename T>
template<typename U>
asset_ref<T> &asset_ref<T>::operator=(const asset_ref<U> &other) noexcept {
	destruct();
	owner = other.owner;
	construct();
	return *this;
}

template<typename T>
template<typename U>
asset_ref<T> &asset_ref<T>::operator=(asset_ref<U> &&other) noexcept {
	std::swap(owner, other.owner);
	return *this;
}

template<typename T>
asset_ref<T>::operator bool() const noexcept {
	return owner && owner->ptr;
}

template<typename T>
T *asset_ref<T>::operator->() const {
	return static_cast<T *>(owner->ptr.get());
}

template<typename T>
T *asset_ref<T>::get() const {
	return operator->();
}

template<typename T>
bool asset_ref<T>::valid() const noexcept {
	return *this;
}

template<typename T>
void asset_ref<T>::construct() noexcept {
	if (!owner)
		return;

	(owner->count)++;
}

template<typename T>
void asset_ref<T>::destruct() noexcept {
	if (!owner)
		return;

	if (--(owner->count) == 0)
		delete owner;
}

}

namespace volt::assets::_internal {

asset_owner::asset_owner(std::unique_ptr<asset> &&ptr) : ptr(std::move(ptr)) {
	VOLT_ASSERT(this->ptr, "Pointer must be a valid address.");
}

asset_owner::~asset_owner() {
	path_to_cached_owner.erase(cache_it);
}

}
