#pragma once

#include "../macros.hpp"

#include <cstddef>
#include <typeindex>
#include <string>

#include "../modules.hpp"

#define VOLT_COMPONENT
#define VOLT_SYSTEM

namespace volt::ecs {

VOLT_API const std::unordered_map<std::string, std::vector<std::string>> &component_names();

VOLT_API const std::unordered_map<std::string, std::vector<std::string>> &system_names();

template<typename T>
const std::string &component_name();

}

namespace volt::ecs::_internal {

class base_component_storage;

VOLT_API extern std::unordered_map<std::string, std::vector<std::string>> module_name_to_component_names;
VOLT_API extern std::unordered_map<std::string, std::vector<std::string>> module_name_to_system_names;

VOLT_API extern std::unordered_map<std::type_index, size_t> component_type_index_to_index;
VOLT_API extern std::unordered_map<std::string, size_t> component_name_to_index;

VOLT_API extern std::vector<std::function<base_component_storage *
		()>> component_index_to_storage_constructor;

template<typename T>
void register_component(const std::string &name);

VOLT_API size_t component_count();

#ifdef VOLT_DEVELOPMENT

VOLT_DEVELOPMENT_MODULE_LOAD_CALLBACK VOLT_API void development_module_load_callback();

VOLT_DEVELOPMENT_MODULE_UNLOAD_CALLBACK VOLT_API void development_module_unload_callback();

#endif

}

#include "ecs.inl"

#include "entity.hpp"
#include "world.hpp"
