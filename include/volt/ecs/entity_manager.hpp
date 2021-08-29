#pragma once

#include "../macros.hpp"

#include <bitset>
#include <cstdint>

#include <nlohmann/json.hpp>

#include "../modules/instance.hpp"
#include "component_storage.hpp"

namespace volt::ecs {

class entity_manager {
public:
	friend class entity;

	VOLT_API entity create();

public:
	struct entity_data {
		std::bitset<VOLT_MAX_COMPONENTS> mask;
		uint32_t version = 0;
	};

	std::vector<entity_data> entities;
	std::vector<uint32_t> free_eids;
	std::array<modules::shared_instance<base_component_storage
			>, VOLT_MAX_COMPONENTS> storages;

	VOLT_API bool expired(uint32_t eid, uint32_t version) const;

	VOLT_API void destroy(uint32_t eid);

	template<typename T>
	bool has_component(uint32_t eid) const;

	template<typename T>
	T &get_component(uint32_t eid);

	template<typename T, typename... Args>
	T &add_component(uint32_t eid, Args &&...args);

	template<typename T>
	void remove_component(uint32_t eid);

	VOLT_API bool has_component_index(uint32_t eid, size_t index) const;

	VOLT_API void add_component_index(uint32_t eid, size_t index, const nlohmann::json &json);

	VOLT_API nlohmann::json get_component_index(uint32_t eid, size_t index);

	VOLT_API void remove_component_index(uint32_t eid, size_t index);

	VOLT_API base_component_storage *get_or_init_storage(size_t index);
};

}

#include "entity_manager.inl"
