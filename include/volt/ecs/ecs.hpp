#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <map>
#include <typeindex>
#include <vector>

namespace volt::ecs {

VOLT_API extern std::vector<std::type_index> _component_to_type_index;
VOLT_API extern std::map<std::type_index, size_t> _type_to_component_index;
VOLT_API extern std::vector<std::string> _component_index_to_name;
VOLT_API extern std::map<std::string, size_t> _component_name_to_index;

VOLT_API std::string _make_component_storage_name(const std::string &component_name);

template<typename T>
void register_component(const std::string &name);

VOLT_API void unregister_component(const std::string &name);

template<typename T>
const std::string &get_component_name();

}

#include "ecs.inl"
