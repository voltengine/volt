#include "../modules.hpp"

namespace volt::assets {

template<std::derived_from<asset> T>
void register_type(const std::string &name) {
	using namespace _internal;

	VOLT_ASSERT(!type_to_constructor.contains(name),
			"Asset type \"" + name + "\" is already registered.")

	type_to_constructor[name] = []() {
		return new T;
	};

	module_name_to_types[modules::this_module_name()].emplace(name);
	type_index_to_type[typeid(T)] = name;
}

}
