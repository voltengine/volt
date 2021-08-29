#pragma once

#include "../modules/modules.hpp"

#include <concepts>
#include <cstdint>
#include <vector>

#include <nlohmann/json.hpp>

// #include "../modules/serializable.hpp"

namespace volt::ecs {

class base_component_storage : public modules::serializable {
public:
	virtual ~base_component_storage() = default;

	VOLT_API uint32_t get_cid(uint32_t eid) const;

	VOLT_API uint32_t get_eid(uint32_t cid) const;

	VOLT_API virtual nlohmann::json serialize() const override;

	VOLT_API virtual void deserialize(const nlohmann::json &json) override;

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

	const std::vector<T> &get_components() const;

	nlohmann::json serialize() const override;

	void deserialize(const nlohmann::json &json) override;

	virtual void add_json(uint32_t eid, const nlohmann::json &json) override;

	virtual nlohmann::json get_json(uint32_t cid) const override;

	void remove(uint32_t cid) override;

private:
	// TODO: Implement custom bucket storage
	// like deque to reduce add() overhead
	std::vector<T> components;
};

}

#include "component_storage.inl"
