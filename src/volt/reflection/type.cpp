#include <volt/pch.hpp>
#include <volt/reflection/type.hpp>

namespace volt::reflection {

type &type::get(std::type_index index) {
	return *index_to_type.at(index);
}

type &type::get(const std::string &name) {
	return *name_to_type.at(name);
}

const std::list<weak_instance<>> &type::get_instances() const {
	return instances;
}

std::type_index type::get_index() const {
	return index;
}

const std::string &type::get_name() const {
	return name;
}

std::map<std::type_index, std::unique_ptr<type>> type::index_to_type;
std::map<std::string, type *> type::name_to_type;

type::type(std::type_index index, std::string_view name)
		: index(index), name(name) {}

}
