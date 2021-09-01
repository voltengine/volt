#pragma once

#include "../macros.hpp"

#include <list>
#include <memory>
#include <type_traits>

#include "serializable.hpp"

namespace volt::modules {

class _instance_owner {
public:
	serializable *ptr;
	size_t shared_count = 0, weak_count = 0;
	
	std::list<_instance_owner *> &tracking_list;
	std::list<_instance_owner *>::iterator it;

	inline _instance_owner(serializable *ptr, std::list<_instance_owner *> &tracking_list);
};

template<typename T, bool Weak>
class _instance {
public:
	template<typename T, bool Weak>
	friend class _instance;
	
	_instance(_instance_owner *owner = nullptr);

	template<typename OtherT, bool OtherWeak>
	_instance(const _instance<OtherT, OtherWeak> &other) noexcept;

	template<typename OtherT, bool OtherWeak>
	_instance(_instance<OtherT, OtherWeak> &&other) noexcept;

	~_instance() noexcept;

	template<typename OtherT, bool OtherWeak>
	_instance<T, Weak> &operator=(const _instance<OtherT, OtherWeak> &other) noexcept;

	template<typename OtherT, bool OtherWeak>
	_instance<T, Weak> &operator=(_instance<OtherT, OtherWeak> &&other) noexcept;

	operator bool() const noexcept;

	T *operator*() const;

	T *operator->() const;

	T *get() const;

	bool valid() const noexcept;

private:
	_instance_owner *owner;

	void construct() noexcept;

	void destruct() noexcept;
};

template<typename T>
concept _serializable_type = std::is_base_of_v<serializable, T>;

template<_serializable_type T = serializable>
using shared_instance = _instance<T, false>;

template<_serializable_type T = serializable>
using weak_instance = _instance<T, true>;

}

#include "instance.inl"
