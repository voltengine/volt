#pragma once

#include "../macros.hpp"

#include <concepts>
#include <string_view>
#include <type_traits>

namespace volt::reflection {

class type;

template<typename T>
concept _field_pointer = std::is_member_object_pointer_v<T>;

template<typename T>
concept _method_pointer = std::is_member_function_pointer_v<T>;

template<typename T>
class builder {
public:
	builder(type *type);

	template<typename... Args>
	builder<T> &constructor();

	template<_field_pointer FieldPointer>
	builder<T> &field(const std::string &name, FieldPointer ptr);

	template<_method_pointer MethodPointer>
	builder<T> &method(const std::string &name, MethodPointer ptr);

private:
	type *type;
};

}

#include "builder.inl"
