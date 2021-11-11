#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <typeindex>
#include <vector>

#include "../modules.hpp"

#define VOLT_COMPONENT
#define VOLT_SYSTEM

namespace volt::ecs {

class system;

template<typename T>
concept system_type = std::is_base_of_v<system, T>;

template<typename T>
void register_component(const std::string &name);

template<system_type T>
void register_system(const std::string &name);

template<typename T>
const std::string &get_component_name();

template<typename T>
const std::string &get_system_name();

VOLT_API size_t get_component_count();

VOLT_API const std::map<std::string, std::set<std::string>> &get_component_names();

VOLT_API const std::map<std::string, std::set<std::string>> &get_system_names();

VOLT_API void update_systems(float delta);

}

namespace volt::ecs::_internal {

class base_component_storage;

VOLT_API extern std::map<std::type_index, std::string> component_type_index_to_name;
VOLT_API extern std::map<std::type_index, std::string> system_type_index_to_name;

VOLT_API extern std::map<std::string, size_t> component_name_to_index;
VOLT_API extern std::map<std::type_index, size_t>
		component_type_index_to_index; // Not necessary, kept just for lookup speed

VOLT_API extern std::map<std::string, std::function<base_component_storage *
		()>> component_name_to_storage_constructor;
VOLT_API extern std::map<std::string, std::function<system *
		()>> system_name_to_constructor;

VOLT_API extern std::map<std::string, std::
		unique_ptr<system>> system_instances;

// Module stuff
VOLT_API extern std::map<std::string, std::set<
		std::string>> module_name_to_component_names;
VOLT_API extern std::map<std::string, std::set<
		std::string>> module_name_to_system_names;

template<typename T>
size_t get_component_index();

size_t get_component_index(const std::string &name);

#ifdef VOLT_DEVELOPMENT

VOLT_MODULE_LOAD_CALLBACK VOLT_API void module_load_callback(const std::string &module_name);

#endif

VOLT_MODULE_UNLOAD_CALLBACK VOLT_API void module_unload_callback(const std::string &module_name);

}

#include "ecs.inl"

#include "entity.hpp"
#include "system.hpp"
#include "world.hpp"
