#pragma once

#include "../macros.hpp"

#include <cstddef>
#include <typeindex>
#include <string>
#include <unordered_map>

#include "../modules.hpp"
#include "../util/util.hpp"

#define VOLT_COMPONENT
#define VOLT_SYSTEM

namespace volt::ecs {

class system;

}

namespace volt::ecs::_internal {

class base_component_storage;

VOLT_API extern std::unordered_map<std::type_index, size_t> component_type_index_to_index;
VOLT_API extern std::unordered_map<std::type_index, size_t> system_type_index_to_index;

VOLT_API extern std::unordered_map<std::string, size_t> component_name_to_index;
VOLT_API extern std::unordered_map<std::string, size_t> system_name_to_index;

VOLT_API extern std::vector<std::function<base_component_storage *
		()>> component_index_to_storage_constructor;
VOLT_API extern std::vector<std::function<system *
		()>> system_index_to_constructor;

#ifdef VOLT_DEVELOPMENT

VOLT_API extern std::unordered_map<std::string, std::vector<std::string>> module_name_to_component_names;
VOLT_API extern std::unordered_map<std::string, std::vector<std::string>> module_name_to_system_names;

#endif

VOLT_API void update_systems(float delta);

VOLT_API size_t component_count();

VOLT_API size_t system_count();

#ifdef VOLT_DEVELOPMENT
// Used to diplay meaningful error messages

VOLT_API const std::string &component_name(size_t index);

VOLT_API const std::string &system_name(size_t index);

#endif

template<typename T>
void register_component(const std::string &module_name, const std::string &name);

template<typename T>
void register_system(const std::string &module_name, const std::string &name);

#ifdef VOLT_DEVELOPMENT

VOLT_DEVELOPMENT_MODULE_LOAD_CALLBACK VOLT_API void development_module_load_callback();

VOLT_DEVELOPMENT_MODULE_UNLOAD_CALLBACK VOLT_API void development_module_unload_callback();

#endif

VOLT_UNLOAD_ALL_MODULES_CALLBACK VOLT_API void unload_all_modules_callback();

}

#include "ecs.inl"

#include "entity.hpp"
#include "system.hpp"
#include "world.hpp"
