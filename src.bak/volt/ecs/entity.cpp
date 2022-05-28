#include <volt/pch.hpp>
#include <volt/ecs/entity.hpp>

#include <volt/util/string.hpp>

#include <volt/error.hpp>

namespace volt::ecs {

const entity entity::null;

entity::entity() : world(nullptr) {}

std::ostream &operator<<(std::ostream &lhs, entity rhs) {
	if (!rhs.world)
		return lhs << "Null Entity";

	lhs << "Entity #" << rhs.id;

	if (!rhs)
		lhs << " (Expired)";
	
	return lhs;
}

entity::operator bool() const {
	return world && !world->expired(id, version);
}

void entity::destroy() const {
	VOLT_DEVELOPMENT_ASSERT(*this, "Cannot access " + util::to_string(*this))
	world->destroy(id);
}

bool entity::has(const std::string &name) const {
	VOLT_DEVELOPMENT_ASSERT(*this, "Cannot access " + util::to_string(*this))
	return world->has_component_by_name(id, name);
}

nlohmann::json entity::get(const std::string &name) const {
	VOLT_DEVELOPMENT_ASSERT(has(name), util::to_string(*this) +
			" does not contain " + name + '.')
	return world->get_component_by_name(id, name);
}

void entity::add(const std::string &name, const nlohmann::json &json) const {
	VOLT_DEVELOPMENT_ASSERT(!has(name), util::to_string(*this) +
			" already contains " + name + '.')
	world->add_component_by_name(id, name, json);
}

void entity::remove(const std::string &name) const {
	VOLT_DEVELOPMENT_ASSERT(has(name), util::to_string(*this) +
			" does not contain " + name + '.')
	world->remove_component_by_name(id, name);
}

entity::entity(ecs::world *world, uint32_t id, uint32_t version)
		: world(world), id(id), version(version) {}

}
