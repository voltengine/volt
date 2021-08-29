#pragma once

#include "../macros.hpp"

#include <list>
#include <memory>

#include "serializable.hpp"

namespace volt::modules {

class _instance_owner {
public:
	serializable *ptr;

	inline _instance_owner(serializable *ptr, std::list<_instance_owner *> &tracking_list);

	inline ~_instance_owner() noexcept;

private:
	std::list<_instance_owner *> &tracking_list;
	std::list<_instance_owner *>::iterator it;
};

class _ref_manager {
public:
	_instance_owner *owner;
	size_t shared_count = 0, weak_count = 0;

	inline _ref_manager(_instance_owner *owner) noexcept;

	inline void construct_shared() noexcept;

	inline void destruct_shared();

	inline void construct_weak() noexcept;

	inline void destruct_weak() noexcept;

	inline bool owner_valid() const noexcept;
};

template<typename T = serializable>
class shared_instance {
public:
	template<typename T>
	friend class shared_instance;

	template<typename T>
	friend class weak_instance;

	shared_instance();

	shared_instance(_instance_owner *owner);

	template<typename U>
	shared_instance(const shared_instance<U> &other) noexcept;

	template<typename U>
	shared_instance(shared_instance<U> &&other) noexcept;

	~shared_instance() noexcept;

	template<typename U>
	shared_instance<T> &operator=(const shared_instance<U> &other) noexcept;

	template<typename U>
	shared_instance<T> &operator=(shared_instance<U> &&other) noexcept;

	operator bool() const noexcept;

	T *operator*() const;

	T *operator->() const;

	T *get() const;

private:
	_ref_manager *manager;

	shared_instance(_ref_manager *manager);
};

template<typename T = serializable>
class weak_instance {
public:
	template<typename T>
	friend class weak_instance;

	template<typename U>
	weak_instance(const weak_instance<U> &other) noexcept;

	template<typename U>
	weak_instance(weak_instance<U> &&other) noexcept;

	template<typename U>
	weak_instance(const shared_instance<U> &other) noexcept;

	~weak_instance() noexcept;

	template<typename U>
	weak_instance<T> &operator=(const weak_instance<U> &other) noexcept;

	template<typename U>
	weak_instance<T> &operator=(weak_instance<U> &&other) noexcept;

	template<typename U>
	operator shared_instance<U>() const noexcept;

	bool valid() const noexcept;

private:
	_ref_manager *manager;
};

}

#include "instance.inl"
