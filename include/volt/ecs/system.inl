#include <memory>
#include <stack>
#include <string>

#include "../error.hpp"

namespace volt::ecs {

template<typename T>
void async_system::unique_lock() {
	_unique_mask.set(_get_component_index<T>());
}

template<typename T>
void async_system::shared_lock() {
	_shared_mask.set(_get_component_index<T>());
}

template<typename T>
void async_system::depend() {
	_dependencies.emplace(typeid(T));

#ifdef VOLT_DEVELOPMENT
	using namespace _internal;

	std::string &root_name = get_system_name<T>();
	std::stack<std::pair<std::string, std::unique_ptr<async_system>>> deps_to_check;

	deps_to_check.emplace(root_name, system_name_to_constructor[root_name]());
	while (!deps_to_check.empty()) {
		auto &item = deps_to_check.top();
		std::string path = std::move(item.first);
		std::unique_ptr<async_system> system = std::move(item.second);
		deps_to_check.pop();

		for (auto dep : system->_dependencies) {
			auto &dep_name = system_type_index_to_name[dep];
			path += " -> " + dep_name;
			VOLT_ASSERT(dep_name != root_name, "Asynchronous system"
					" circular dependency detected:\n" + path);
			deps_to_check.emplace(path, system_name_to_constructor[dep_name]());
		}
	}
#endif
}

}
