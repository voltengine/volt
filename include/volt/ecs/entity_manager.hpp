#pragma once

#include "../macros.hpp"

#include <bitset>
#include <cstdint>
#include <list>
#include <vector>

#include <nlohmann/json.hpp>

#include "../modules/instance.hpp"
#include "component_storage.hpp"

namespace volt::ecs {

class entity_manager {
public:
	friend class entity;

	struct _entity_data {
		std::bitset<VOLT_MAX_COMPONENTS> mask;
		uint32_t version = 0;
	};

	VOLT_API static std::list<entity_manager *> _instances;
	
	std::vector<_entity_data> _entities;
	std::array<modules::shared_instance<_base_component_storage
			>, VOLT_MAX_COMPONENTS> _storages;

	VOLT_API entity_manager();

	VOLT_API ~entity_manager();

	VOLT_API entity create();

private:
	std::list<entity_manager *>::iterator instance_it;
	std::vector<uint32_t> free_eids;

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

	VOLT_API _base_component_storage *get_or_init_storage(size_t index);
};

}

#include "entity_manager.inl"
