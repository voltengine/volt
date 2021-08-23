#pragma once

#include "../macros.hpp"

#include <list>
#include <memory>

namespace volt::reflection {

class serializable;

class type;

template<typename T>
class weak_instance;

class _instance_owner {
public:
	serializable *ptr;
	type &type;
	std::list<weak_instance<serializable>>::iterator it;

	inline _instance_owner(serializable *ptr, reflection::type &type) noexcept;

	inline ~_instance_owner() noexcept;

	template<typename T>
	T &get(const std::string &field_name) const {
		std::any &any_accessor = type.fields[field_name];
		auto *accessor = std::any_cast<std::function<T &
				(serializable *)>>(&any_accessor);
		return (*accessor)(ptr);
	}
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

	shared_instance(_instance_owner *owner);

	template<typename U>
	shared_instance(const shared_instance<U> &other) noexcept;

	template<typename U>
	shared_instance(shared_instance<U> &&other) noexcept;

	~shared_instance() noexcept;

	T *operator->() const;

	type &get_type() const noexcept;

	template<typename T>
	T &get(const std::string &field_name) const {
		return manager->owner->get<T>(field_name);
	}

	// template<typename T>
	// T &get(const std::string &field_name);

	// template<typename Ret, typename... Args>
	// Ret call(const std::string &method_name, Args &&...args);

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

	type &get_type() const noexcept;

private:
	_ref_manager *manager;

	weak_instance(_ref_manager *manager) noexcept;
};

}

#include "instance.inl"
