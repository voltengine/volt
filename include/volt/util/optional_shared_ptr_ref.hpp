#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::util {

template<typename T>
class optional_shared_ptr_ref {
public:
	optional_shared_ptr_ref() = default;

	optional_shared_ptr_ref(const std::shared_ptr<T> &ptr);

	template<typename U>
	operator U *() const;

	operator bool() const;

	T &operator*() const;

	T *operator->() const;

private:
	T *ptr = nullptr;
};

}

#include "optional_shared_ptr_ref.inl"
