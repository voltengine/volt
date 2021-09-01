#include <volt/ecs/ecs.hpp>

#include <volt/ecs/entity_manager.hpp>

namespace volt::ecs {

std::vector<std::type_index> _component_to_type_index;
std::map<std::type_index, size_t> _type_to_component_index;
std::vector<std::string> _component_index_to_name;
std::map<std::string, size_t> _component_name_to_index;

std::string _make_component_storage_name(const std::string &component_name) {
	return "volt::ecs::_component_storage<" + component_name + '>';
}

void unregister_component(const std::string &name) {
	if (!_component_name_to_index.contains(name))
		throw std::runtime_error("No such component \"" + name + "\" is registered.");
	
	size_t index = _component_name_to_index[name];
	size_t last_index = _component_name_to_index.size() - 1;

	std::swap(_component_to_type_index[index], _component_to_type_index[last_index]);
	_type_to_component_index.erase(_component_to_type_index[last_index]);
	_component_to_type_index.pop_back();

	std::swap(_component_index_to_name[index], _component_index_to_name[last_index]);
	_component_index_to_name.pop_back();
	_component_name_to_index.erase(name);

	for (auto manager : entity_manager::_instances) {
		std::swap(manager->_storages[index], manager->_storages[last_index]);
		manager->_storages[last_index] = nullptr;
	}

	modules::unregister_serializable(_make_component_storage_name(name));
}

}