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

	VOLT_API uint32_t cid(uint32_t eid) const;

	VOLT_API uint32_t eid(uint32_t cid) const;

	virtual void add_json(uint32_t eid, const nlohmann::json &json) = 0;

	virtual nlohmann::json get_json(uint32_t cid) const = 0;

	virtual void remove(uint32_t cid) = 0;

protected:
	std::vector<uint32_t> eid_to_cid, cid_to_eid;
};

template<typename T>
class component_storage : public base_component_storage {
public:
	template<typename... Args>
	T &add(uint32_t eid, Args &&...args);

	T &get(uint32_t cid);

	const T &get(uint32_t cid) const;

	const std::vector<T> &components() const;

	virtual void add_json(uint32_t eid, const nlohmann::json &json) override;

	virtual nlohmann::json get_json(uint32_t cid) const override;

	void remove(uint32_t cid) override;

private:
	std::vector<T> _components;
};

}

#include "component_storage.inl"
