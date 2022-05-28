#pragma once

#include "../modules.hpp"

#include <concepts>
#include <cstdint>
#include <vector>

#include <nlohmann/json.hpp>

namespace volt::ecs::_internal {

class base_component_storage {
public:
	virtual ~base_component_storage() = default;

	virtual void add_json(const nlohmann::json &json) = 0;

	virtual nlohmann::json get_json(uint32_t cid) const = 0;

	virtual void remove(uint32_t cid) = 0;

	virtual void copy_from(const base_component_storage &storage, size_t cid) = 0;
};

template<typename T>
class component_storage : public base_component_storage {
public:
	std::vector<T> components;

	template<typename... Args>
	T &add(Args &&...args);

	T &get(uint32_t cid);

	const T &get(uint32_t cid) const;

	virtual void add_json(const nlohmann::json &json) override;

	virtual nlohmann::json get_json(uint32_t cid) const override;

	void remove(uint32_t cid) override;

	void copy_from(const base_component_storage &storage, size_t cid) override;
};

}

#include "component_storage.inl"
