#pragma once

#include "../macros.hpp"

#include <concepts>
#include <cstdint>
#include <map>
#include <type_traits>
#include <vector>

#include "../serializable.hpp"

namespace volt::ecs {

class base_component_storage;

template<typename T>
concept component_type = std::is_base_of_v<serializable, T>;

extern size_t _component_index_counter;
extern std::vector<std::function<std::unique_ptr<
		base_component_storage>()>> _storage_constructors;
extern std::vector<std::string> _component_index_to_name;
extern std::map<std::string, size_t> _component_name_to_index;
extern std::list<instance_owner<base_component_storage> *> _storage_instances;
extern std::map<std::string, size_t> _module_to_component_index;
extern bool _added_module_reload_callback;
extern auto _module_reload_callback = [](auto &name) {
	size_t index = _module_to_component_index[name];
	_storage_constructors[index] = nullptr;
	return [name]() {

	};
}

template<component_type T>
void register_component(const std::string &name);

template<component_type T>
size_t get_component_index();

size_t get_component_index(const std::string &name);

shared_instance<base_component_storage> make_storage(size_t index);

const std::string &get_component_name(size_t index);

}

#include "ecs.inl"
