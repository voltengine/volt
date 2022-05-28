#include <volt/pch.hpp>
#include <volt/ecs/ecs.hpp>

namespace volt::ecs::_internal {

std::unordered_map<std::type_index, size_t> component_type_index_to_index;
std::unordered_map<std::string, size_t> component_name_to_index;

std::vector<size_t> component_index_to_size;
std::vector<std::function<base_component_storage *
		()>> component_index_to_storage_constructor;

size_t component_count() {
	return component_index_to_size.size();
}


}
