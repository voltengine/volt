#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <typeindex>
#include <vector>

namespace volt::ecs {

class _base_component_storage;

class system;

template<typename T>
concept system_type = std::is_base_of_v<system, T>;

VOLT_API extern std::map<std::type_index, std::string> _component_type_index_to_name;

VOLT_API extern std::map<std::string, size_t> _component_name_to_index;
VOLT_API extern std::map<std::type_index, size_t>
		_component_type_index_to_index; // Not necessary, kept just for lookup speed

VOLT_API extern std::map<std::string, std::function<_base_component_storage *
		()>> _component_name_to_storage_constructor;
VOLT_API extern std::map<std::string, std::function<system *
		()>> _system_name_to_constructor;

VOLT_API extern std::map<std::string, std::
		unique_ptr<system>> _system_instances;

// Module stuff
VOLT_API extern std::map<std::string, std::set<
		std::string>> _module_name_to_component_names;
VOLT_API extern std::map<std::string, std::set<
		std::string>> _module_name_to_system_names;

VOLT_API void _module_unload_callback(const std::string &module_name, bool remember_state);

template<typename T>
void register_component(const std::string &name);

template<system_type T>
void register_system(const std::string &name);

template<typename T>
const std::string &get_component_name();

VOLT_API size_t get_component_count();

VOLT_API const std::map<std::string, std::set<std::string>> &get_component_names();

VOLT_API const std::map<std::string, std::set<std::string>> &get_system_names();

VOLT_API void update_systems(float delta);

}

#include "ecs.inl"
