#include "../assert.hpp"

namespace volt::modules {

// _instance_owner

_instance_owner::_instance_owner(serializable *ptr,
		std::list<_instance_owner *> &tracking_list)
		: ptr(ptr), tracking_list(tracking_list) {
	VOLT_ASSERT(ptr, "Pointer must be a valid address.");
	// Pointer can actually become NULL when modules were reloaded
	// and serializable type has not been registered back,
	// so we always check for that in _instance
	it = tracking_list.emplace(tracking_list.end(), this);
}

// _instance

template<typename T, bool Weak>
_instance<T, Weak>::_instance(_instance_owner *owner)
		: owner(owner) {
	construct();
}

template<typename T, bool Weak>
template<typename OtherT, bool OtherWeak>
_instance<T, Weak>::_instance(const _instance<OtherT, OtherWeak> &other) noexcept
		: _instance(other.owner) {}

template<typename T, bool Weak>
template<typename OtherT, bool OtherWeak>
_instance<T, Weak>::_instance(_instance<OtherT, OtherWeak> &&other) noexcept
		: owner(other.owner) {
	other.owner = nullptr;
}

template<typename T, bool Weak>
_instance<T, Weak>::~_instance() noexcept {
	destruct();
}

template<typename T, bool Weak>
template<typename OtherT, bool OtherWeak>
_instance<T, Weak> &_instance<T, Weak>::operator=(const _instance<OtherT, OtherWeak> &other) noexcept {
	destruct();
	owner = other.owner;
	construct();
	return *this;
}

template<typename T, bool Weak>
template<typename OtherT, bool OtherWeak>
_instance<T, Weak> &_instance<T, Weak>::operator=(_instance<OtherT, OtherWeak> &&other) noexcept {
	std::swap(owner, other.owner);
	return *this;
}

template<typename T, bool Weak>
_instance<T, Weak>::operator bool() const noexcept {
	return owner && owner->ptr;
}

template<typename T, bool Weak>
T *_instance<T, Weak>::operator->() const {
	return static_cast<T *>(owner->ptr);
}

template<typename T, bool Weak>
T *_instance<T, Weak>::get() const {
	return operator->();
}

template<typename T, bool Weak>
bool _instance<T, Weak>::valid() const noexcept {
	return *this;
}

template<typename T, bool Weak>
void _instance<T, Weak>::construct() noexcept {
	if (!owner)
		return;

	if constexpr (Weak)
		(owner->weak_count)++;
	else
		(owner->shared_count)++;
}

template<typename T, bool Weak>
void _instance<T, Weak>::destruct() noexcept {
	if (!owner)
		return;

	if constexpr (Weak) {
		if (--(owner->weak_count) == 0
				&& owner->shared_count == 0)
			delete owner;
	} else {
		if (--(owner->shared_count) == 0) {
			if (owner->ptr) {
				// If ptr is NULL, then tracking list does not exist anymore
				owner->tracking_list.erase(owner->it);
				delete owner->ptr;
			}
			if (owner->weak_count == 0)
				delete owner;
		}
	}
}

}
