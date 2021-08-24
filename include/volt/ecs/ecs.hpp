#pragma once

#include "../macros.hpp"

#include <concepts>
#include <cstdint>
#include <map>
#include <type_traits>
#include <vector>

#include "../instance.hpp"
#include "../modules.hpp"
#include "../serializable.hpp"
#include "component_storage.hpp"

namespace volt::ecs {

VOLT_API extern size_t _component_index_counter;
VOLT_API extern std::vector<std::function<
		base_component_storage *()>> _storage_constructors;
VOLT_API extern std::vector<std::string> _component_index_to_name;
VOLT_API extern std::map<std::string, size_t> _component_name_to_index;
VOLT_API extern std::vector<std::list<instance_owner *>> _storage_owner_lists;
VOLT_API extern std::map<std::string, size_t> _module_name_to_component_index;
VOLT_API extern bool _added_module_reload_callback;
VOLT_API extern modules::reload_callback _module_reload_callback;

template<typename T>
void register_component(const std::string &name);

size_t get_component_index(const std::string &name);

const std::string &get_component_name(size_t index);

shared_instance<base_component_storage> make_storage(size_t index);

}

#include "ecs.inl"
