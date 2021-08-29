#include <volt/ecs/ecs.hpp>

namespace volt::ecs {

size_t _component_index_counter = 0;
std::vector<std::type_index> _component_to_type_index;
std::map<std::type_index, size_t> _type_to_component_index;
std::vector<std::string> _component_index_to_name;
std::map<std::string, size_t> _component_name_to_index;

std::string _make_component_storage_name(const std::string &component_name) {
	return "volt::ecs::component_storage<" + component_name + '>';
}

}