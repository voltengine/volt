#include <volt/pch.hpp>
#include <volt/ecs/entity.hpp>

#include <volt/util/string.hpp>

namespace volt::ecs {

const entity entity::null;

entity::entity() : manager(nullptr) {}

std::ostream &operator<<(std::ostream &lhs, entity rhs) {
	if (!rhs.manager)
		return lhs << "Null Entity";

	lhs << "Entity #" << rhs.id;

	if (!rhs)
		lhs << " (Expired)";
	
	return lhs;
}

entity::operator bool() const {
	return manager && !manager->expired(id, version);
}

void entity::destroy() const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));

	manager->destroy(id);
}

bool entity::has(const std::string &name) const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));

	return manager->has_component_index(id, _component_name_to_index[name]);
}

nlohmann::json entity::get(const std::string &name) const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	if (!has(name))
		throw std::runtime_error(util::to_string(*this) + " does not contain " + name + '.');

	return manager->has_component_index(id, _component_name_to_index[name]);
}

void entity::add(const std::string &name, const nlohmann::json &json) const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	if (has(name))
		throw std::runtime_error(util::to_string(*this) + " already contains " + name + '.');
	
	manager->add_component_index(id, _component_name_to_index[name], json);
}

void entity::remove(const std::string &name) const {
	if (!*this)
		throw std::runtime_error("Cannot access " + util::to_string(*this));
	if (!has(name))
		throw std::runtime_error(util::to_string(*this) + " does not contain " + name + '.');
	
	manager->remove_component_index(id, _component_name_to_index[name]);
}

entity::entity(entity_manager *manager, uint32_t id, uint32_t version)
		: manager(manager), id(id), version(version) {}


}
