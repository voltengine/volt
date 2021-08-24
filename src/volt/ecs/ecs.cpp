#include <volt/ecs/ecs.hpp>

namespace nl = nlohmann;

namespace volt::ecs {

size_t _component_index_counter = 0;
std::vector<std::string> _component_index_to_name;
std::map<std::string, size_t> _component_name_to_index;

std::vector<std::function<
		base_component_storage *()>> _storage_constructors;
std::vector<std::list<instance_owner *>> _storage_owner_lists;

std::map<std::string, size_t> _module_name_to_component_index;
bool _added_module_reload_callback = false;
modules::reload_callback _module_reload_callback =
		[](auto &name) -> modules::load_callback {
	size_t index = _module_name_to_component_index[name];
	_storage_constructors[index] = nullptr;

	std::vector<nl::json> serialized_data;

	auto &owner_list = _storage_owner_lists[index];
	for (auto &owner : _storage_owner_lists[index]) {
		serialized_data.emplace_back(owner->ptr->serialize());
		delete owner->ptr;
	}

	return [serialized_data = std::move(serialized_data),
			index]() {
		auto &owner_list = _storage_owner_lists[index];
		for (auto it = owner_list.begin(); it != owner_list.end(); it++)
			(*it)->ptr = _storage_constructors[index](); 
	};
};

size_t get_component_index(const std::string &name) {
	return _component_name_to_index[name];
}

const std::string &get_component_name(size_t index) {
	return _component_index_to_name[index];
}

shared_instance<base_component_storage> make_storage(size_t index) {
	base_component_storage *storage = _storage_constructors[index]();
	auto owner = new instance_owner(storage, _storage_owner_lists[index]);
	return shared_instance<base_component_storage>(owner);
}

}