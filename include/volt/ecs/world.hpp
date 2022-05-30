#pragma once

#include "../macros.hpp"

#include <bitset>
#include <cstdint>
#include <functional>
#include <list>
#include <vector>
#include <type_traits>

#include <nlohmann/json.hpp>

#include "../util/util.hpp"
#include "component_storage.hpp"

namespace volt::ecs::_internal {

class archetype {
public:
    class edge {
    public:
        size_t add = std::numeric_limits<size_t>::max();
        size_t remove = std::numeric_limits<size_t>::max();
    };

    // Archetype properties
    util::dynamic_bitset mask;
    std::vector<edge> edges;

    // Entity data
    size_t size = 0;
	std::vector<size_t> eid_to_cid, cid_to_eid;
    std::vector<std::unique_ptr<base_component_storage>> storages;

	VOLT_API archetype();

	VOLT_API archetype(const util::dynamic_bitset &mask);

	archetype(archetype &&other) = default;

	archetype &operator=(archetype &&other) = default;
};

};

namespace volt::ecs {

class entity;

class world {
public:
	friend entity;

	VOLT_API world();

	VOLT_API ~world();

	VOLT_API entity create();

	VOLT_API entity root();

	template<typename T>
	void each(const std::type_identity_t<std::function<void(T &)>> &func);

	template<typename T>
	void each(const std::type_identity_t<std::function<void(entity, T &)>> &func);

	template<typename T1, typename T2, typename... Tn>
	void each(const std::type_identity_t<std::function<void(T1 &, T2 &, Tn &...)>> &func);

	template<typename T1, typename T2, typename... Tn>
	void each(const std::type_identity_t<std::function<void(entity, T1 &, T2 &, Tn &...)>> &func);

	// template<typename T1, typename... Tn>
	// void each(const std::function<void(T1 &, Tn &...)> &func, util::thread_pool &pool, size_t least_entities_per_job = 50);

#ifdef VOLT_DEVELOPMENT
	// 
	VOLT_API static void _register_development_components();

	VOLT_API static void _unregister_development_components();

	VOLT_API static void _clear_development_reload_snapshots();
#endif

private:
	class entity_data {
	public:
		size_t version = 0; // Entity version to odetermine whether it was removed or not
		size_t archetype; // Index of the archetype in the world's archetypes vector
	};

	// std::list does not invaldiate iterators when it's modified
	// so world instances can always remove themselves from the list
	VOLT_API static std::list<world *> instances;

	std::list<world *>::iterator instance_it;

	std::vector<_internal::archetype> archetypes;
	std::vector<std::vector<size_t>> component_index_to_archetype_indices;

	std::vector<entity_data> entities;
	std::vector<size_t> free_eids;

#ifdef VOLT_DEVELOPMENT
	// Component name -> (eid -> component data serialized as JSON)
	std::unordered_map<std::string, std::unordered_map<uint32_t, nlohmann::json>> development_reload_snapshot;
#endif

	VOLT_API bool expired(size_t eid, size_t version) const;

	VOLT_API void destroy(size_t eid);

	template<typename T>
	bool has_component(size_t eid) const;

	template<typename T>
	T &get_component(size_t eid);

	template<typename T, typename... Args>
	T &add_component(size_t eid, Args &&...args);

	template<typename T>
	void remove_component(size_t eid);

	VOLT_API bool has_component_by_name(size_t eid, const std::string &name) const;

	VOLT_API nlohmann::json get_component_by_name(size_t eid, const std::string &name) const;

	VOLT_API void add_component_by_name(size_t eid, const std::string &name, const nlohmann::json &json);

	VOLT_API void remove_component_by_name(size_t eid, const std::string &name);
};

}

#include "world.inl"
