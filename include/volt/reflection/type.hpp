#pragma once

#include "../macros.hpp"

#include <any>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <typeindex>
#include <vector>

namespace volt::reflection {

class serializable;

template<typename T>
class shared_instance;

template<typename T>
class weak_instance;

class type {
public:
	friend class _instance_owner;

	template<typename T>
	friend class builder;

	using error = std::runtime_error;

	template<typename T>
	static builder<T> define(std::string_view name);

	template<typename T>
	static type &get();

	VOLT_API static type &get(std::type_index index);

	VOLT_API static type &get(const std::string &name);

	template<typename T = serializable, typename... Args>
	shared_instance<T> instantiate(Args &&...args);

	VOLT_API const std::list<weak_instance<serializable>> &get_instances() const;

	VOLT_API std::type_index get_index() const;

	VOLT_API const std::string &get_name() const;

	// VOLT_API const std::vector<constructor> &get_constructors();

	// VOLT_API const std::map<std::string, field> &get_fields();

	// VOLT_API const std::map<std::string, method> &get_constructors();

private:
	VOLT_API static std::map<std::type_index, std::unique_ptr<type>> index_to_type;
	VOLT_API static std::map<std::string, type *> name_to_type;

	// TODO: derived, base
	std::type_index index;
	std::string name;
	std::vector<std::any> constructors;
	std::map<std::string, std::any> fields;
	std::map<std::string, std::any> methods;
	std::list<weak_instance<serializable>> instances;

	VOLT_API type(std::type_index index, std::string_view name);
};

}

#include "type.inl"
