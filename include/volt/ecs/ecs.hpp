#pragma once

#include "../macros.hpp"

#include <cstddef>
#include <typeindex>
#include <string>

#define VOLT_COMPONENT
#define VOLT_SYSTEM

namespace volt::ecs {

template<typename T>
const std::string &component_name();

}

namespace volt::ecs::_internal {

class base_component_storage;

VOLT_API extern std::unordered_map<std::type_index, size_t> component_type_index_to_index;
VOLT_API extern std::unordered_map<std::string, size_t> component_name_to_index;

VOLT_API extern std::vector<size_t> component_index_to_size;
VOLT_API extern std::vector<std::function<base_component_storage *
		()>> component_index_to_storage_constructor;

template<typename T>
void register_component(const std::string &name);

VOLT_API size_t component_count();

}

#include "ecs.inl"

#include "entity.hpp"
#include "world.hpp"
