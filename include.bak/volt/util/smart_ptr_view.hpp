#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::util {

template<typename T>
class smart_ptr_view {
public:
	smart_ptr_view() = default;

	smart_ptr_view(const std::unique_ptr<T> &ptr);

	smart_ptr_view(const std::shared_ptr<T> &ptr);

	smart_ptr_view(const std::weak_ptr<T> &ptr);

	template<typename U>
	operator U *() const;

	operator bool() const;

	T &operator*() const;

	T *operator->() const;

private:
	T *ptr = nullptr;
};

}

#include "smart_ptr_view.inl"
