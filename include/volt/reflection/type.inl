#include <functional>

#include "builder.hpp"
#include "instance.hpp"

namespace volt::reflection {

template<typename T>
builder<T> type::define(std::string_view name) {
	type *type = new reflection::type(typeid(T), name);

	index_to_type.emplace(type->get_index(), type);
	name_to_type.emplace(type->get_name(), type);

	return builder<T>(type);
}

template<typename T>
type &get() {
	return get(typeid(T));
}

template<typename T, typename... Args>
shared_instance<T> type::instantiate(Args &&...args) {
	for (std::any &ctor : constructors) {
		if (ctor.type() != typeid(std::function<serializable *(Args...)>))
			continue;

		auto ctor_func = std::any_cast<std::function<serializable *(Args...)>>(ctor);

		auto *owner = new _instance_owner(ctor_func(
				std::forward<Args>(args)...), *this);
		shared_instance<T> instance(owner);
		owner->it = instances.emplace(instances.end(), instance);
		return instance;
	}

	throw error("No such constructor defined for type " + name + "." );
}

// template<typename T, typename FieldType>
// FieldType &type::get(const std::string &name) {
	
// }

// template<typename T, typename Ret, typename... Args>
// Ret type::call(const std::string &name, Args &&...args) {
// 	return *std::any_cast<Ret (T:: *)(Args ...)>(members.at(name))(std::forward<Args>(args...));
// }

}
