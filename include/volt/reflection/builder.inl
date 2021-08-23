#include <functional>

#include "serializable.hpp"
#include "type.hpp"

namespace volt::reflection {

template<typename T>
struct _field_pointer_traits;

template<typename Class, typename Field>
struct _field_pointer_traits<Field Class:: *> {
	using class_type = Class;
	using field_type = Field;
};

template<typename T>
struct _method_pointer_traits;

template<typename Class, typename Ret, typename... Args>
struct _method_pointer_traits<Ret Class:: *(Args...)> {
	using class_type = Class;
	using return_type = Ret;
	using param_types = std::tuple<Args...>;
};

template<typename T>
builder<T>::builder(reflection::type *type) {
	this->type = type;
}

template<typename T>
template<typename... Args>
builder<T> &builder<T>::constructor() {
	type->constructors.push_back(std::make_any<
			std::function<serializable *(Args...)>>(
		[](Args &&...args) {
			return static_cast<serializable *>(new T(std::forward<Args>(args)...));
		}));

	return *this;
}

template<typename T>
template<_field_pointer FieldPointer>
builder<T> &builder<T>::field(const std::string &name, FieldPointer ptr) {
	using traits = _field_pointer_traits<FieldPointer>;
	using class_type = typename traits::class_type;
	using field_type = typename traits::field_type;

	static_assert(std::is_same_v<T, class_type>,
			"Field pointer does not belong to this type.");

	type->fields[name] = std::make_any<std::function<
			field_type &(serializable *)>>(
		[ptr](serializable *instance) -> field_type & {
			return static_cast<class_type *>(instance)->*ptr;
		});

	return *this;
}

// template<typename T>
// template<_method_pointer MethodPointer>
// builder<T> &builder<T>::method(const std::string &name, MethodPointer ptr) {
// 	using traits = _method_pointer_traits<MethodPointer>;
// 	using class_type = typename traits::class_type;
// 	using return_type = typename traits::field_type;
// 	using param_types = typename traits::param_types;

// 	static_assert(std::is_same_v<T, class_type>,
// 			"Method pointer (registered as \"" + name
// 			+ "\") does not belong to this type.");

// 	type->methods[name] = std::make_any<std::function<
// 			return_type (serializable *, )>>(
// 		[ptr](serializable *instance) -> field_type & {
// 			return static_cast<class_type *>(instance)->*ptr;
// 		});

// 	return *this;
// }

}
