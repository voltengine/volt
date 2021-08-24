#pragma once

#include "macros.hpp"

#include <list>
#include <memory>

#include "serializable.hpp"

namespace volt {

class instance_owner {
public:
	serializable *ptr;

	inline instance_owner(serializable *ptr, std::list<instance_owner *> &parent_list);

	inline ~instance_owner() noexcept;

private:
	std::list<instance_owner *> &parent_list;
	std::list<instance_owner *>::iterator it;
};

class _ref_manager {
public:
	instance_owner *owner;
	size_t shared_count = 0, weak_count = 0;

	inline _ref_manager(instance_owner *owner) noexcept;

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

	shared_instance(instance_owner *owner);

	template<typename U>
	shared_instance(const shared_instance<U> &other) noexcept;

	template<typename U>
	shared_instance(shared_instance<U> &&other) noexcept;

	~shared_instance() noexcept;

	T *operator->() const;

private:
	_ref_manager *manager;

	shared_instance(_ref_manager *manager) noexcept;
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
	operator shared_instance<U>() const noexcept;

	T *operator->() const;

	bool valid() const noexcept;

private:
	_ref_manager *manager;

	weak_instance(_ref_manager *manager) noexcept;
};

}

#include "instance.inl"
