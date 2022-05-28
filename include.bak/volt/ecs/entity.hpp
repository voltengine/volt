#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <ostream>

#include "world.hpp"

namespace volt::ecs {

class entity {
public:
	friend world;

	VOLT_API static const entity null;

	VOLT_API entity();

	VOLT_API friend std::ostream &operator<<(std::ostream &lhs, entity rhs);

	VOLT_API operator bool() const;

	VOLT_API void destroy() const;

	template<typename T>
	bool has() const;

	template<typename T>
	T &get() const;

	template<typename T, typename... Args>
	T &add(Args &&...args) const;

	template<typename T>
	void remove() const;

	VOLT_API bool has(const std::string &name) const;

	VOLT_API nlohmann::json get(const std::string &name) const;

	VOLT_API void add(const std::string &name, const nlohmann::json &json) const;

	VOLT_API void remove(const std::string &name) const;

private:
	world *world;
	uint32_t id, version;

	VOLT_API entity(ecs::world *world, uint32_t id, uint32_t version);

	template<typename T>
	std::string get_type_name();
};

}

#include "entity.inl"
