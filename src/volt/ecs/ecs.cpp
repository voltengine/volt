#include <volt/ecs/ecs.hpp>

namespace volt::ecs {

size_t _component_index_counter = 0;
std::vector<std::function<std::unique_ptr<
		base_component_storage>()>> _storage_constructors;
std::vector<std::string> _component_names;
std::map<std::string, size_t> _component_indices;
bool _added_module_reload_callback = false;

size_t get_component_index(const std::string &name) {
	return _component_name_to_index[name];
}

std::unique_ptr<base_component_storage> make_storage(size_t index) {
	return _storage_constructors[index]();
}

const std::string &get_component_name(size_t index) {
	return _component_index_to_name[index];
}

}