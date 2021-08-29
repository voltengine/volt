#include "../util/string.hpp"

namespace volt::ecs {

template<typename T>
bool entity::has() const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	
	return manager->has_component<T>(id);
}

template<typename T>
T &entity::get() const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	if (!has<T>()) {
		throw std::runtime_error(util::to_string(*this) + " does not contain "
				+ get_component_name<T>() + '.');
	}
	
	return manager->get_component<T>(id);
}

template<typename T, typename... Args>
T &entity::add(Args &&...args) const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	if (has<T>()) {
		throw std::runtime_error(util::to_string(*this) + " already contains "
				+ get_component_name<T>() + '.');
	}
	
	return manager->add_component<T>(id, std::forward<Args>(args)...);
}

template<typename T>
void entity::remove() const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	if (!has<T>()) {
		throw std::runtime_error(util::to_string(*this) + " does not contain "
				+ get_component_name<T>() + '.');
	}
	
	manager->remove_component<T>(id);
}

}
