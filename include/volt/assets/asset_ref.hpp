#pragma once

#include "../macros.hpp"

#include <list>
#include <memory>
#include <concepts>
#include <unordered_map>

namespace volt::assets::_internal {
	class asset_owner;
}

namespace volt::assets {

class asset;

template<typename T>
class asset_ref {
public:
	template<typename T>
	friend class asset_ref;
	
	asset_ref(_internal::asset_owner *owner = nullptr);

	template<typename U>
	asset_ref(const asset_ref<U> &other) noexcept;

	template<typename U>
	asset_ref(asset_ref<U> &&other) noexcept;

	~asset_ref() noexcept;

	template<typename U>
	asset_ref<T> &operator=(const asset_ref<U> &other) noexcept;

	template<typename U>
	asset_ref<T> &operator=(asset_ref<U> &&other) noexcept;

	operator bool() const noexcept;

	T *operator*() const;

	T *operator->() const;

	T *get() const;

	bool valid() const noexcept;

private:
	_internal::asset_owner *owner;

	void construct() noexcept;

	void destruct() noexcept;
};

}

namespace volt::assets::_internal {

class asset_owner {
public:
	using iterator = std::unordered_map<std::string, asset_owner *>::iterator;

	std::unique_ptr<asset> ptr;
	size_t count = 0;
	iterator cache_it;

	inline asset_owner(std::unique_ptr<asset> &&ptr);

	inline ~asset_owner();
};

}

#include "asset_ref.inl"