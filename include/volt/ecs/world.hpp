#pragma once

#include "../macros.hpp"

#include <bitset>
#include <cstdint>
#include <list>
#include <vector>

#include <nlohmann/json.hpp>

#include "component_storage.hpp"

namespace volt::ecs {

class world {
public:
	friend class entity;

	VOLT_API world();

	VOLT_API ~world();

	VOLT_API entity create();

	VOLT_API entity get_root_entity();

	template<typename T>
	void each(const std::function<void(T &)> &func) const;

	template<typename T, typename... Filters>
	void each(const std::function<void(T &, Filters &...)> &func) const;

#ifdef VOLT_DEVELOPMENT
	VOLT_API static void _register_development_component(const std::string &name);

	VOLT_API static void _unregister_development_component(const std::string &name);

	VOLT_API static void _clear_development_reload_snapshots();
#endif

private:
	struct entity_data {
		std::bitset<VOLT_MAX_COMPONENTS> mask;
		uint32_t version = 0;
	};

	VOLT_API static std::list<world *> instances;
	
	std::list<world *>::iterator instance_it;
	std::vector<entity_data> entities;
	std::map<std::string, std::unique_ptr<_internal::base_component_storage>> storages;
	std::vector<uint32_t> free_eids;

#ifdef VOLT_DEVELOPMENT
	std::map<std::string, std::map<uint32_t, nlohmann::json>> development_reload_snapshot;
#endif

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

	VOLT_API bool has_component_by_name(uint32_t eid, const std::string &name) const;

	VOLT_API void add_component_by_name(uint32_t eid, const std::string &name, const nlohmann::json &json);

	VOLT_API nlohmann::json get_component_by_name(uint32_t eid, const std::string &name);

	VOLT_API void remove_component_by_name(uint32_t eid, const std::string &name);
};

}

#include "world.inl"
