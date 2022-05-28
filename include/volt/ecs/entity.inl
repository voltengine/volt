#include "../util/util.hpp"

#include "../error.hpp"
#include "../util/util.hpp"
#include "world.hpp"

namespace volt::ecs {

template<typename T>
bool entity::has() const {
	VOLT_DEVELOPMENT_ASSERT(*this, "Cannot access " + util::to_string(*this))
	return world->has_component<T>(id);
}

template<typename T>
T &entity::get() const {
	VOLT_DEVELOPMENT_ASSERT(has<T>(), util::to_string(*this) +
			" does not contain " + component_name<T>() + '.')
	return world->get_component<T>(id);
}

template<typename T, typename... Args>
T &entity::add(Args &&...args) const {
	VOLT_DEVELOPMENT_ASSERT(!has<T>(), util::to_string(*this) +
			" already contains " + component_name<T>() + '.')
	return world->add_component<T>(id, std::forward<Args>(args)...);
}

template<typename T>
void entity::remove() const {
	VOLT_DEVELOPMENT_ASSERT(has<T>(), util::to_string(*this) +
			" does not contain " + component_name<T>() + '.')
	world->remove_component<T>(id);
}

}
