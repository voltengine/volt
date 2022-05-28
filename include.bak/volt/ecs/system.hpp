#pragma once

#include "../macros.hpp"

#include <bitset>
#include <set>
#include <typeindex>

#include "ecs.hpp"

namespace volt::ecs {

class system {
public:
	bool _async = false;

	virtual ~system() = default;

	virtual void update(float delta) const = 0;
};

class async_system : public system {
public:
	std::set<std::type_index> _dependencies;
	std::bitset<VOLT_MAX_COMPONENTS> _unique_mask, _shared_mask;

	async_system();

	virtual ~async_system() = default;

protected:
	template<typename T>
	void unique_lock();

	template<typename T>
	void shared_lock();

	VOLT_API void unique_lock_all();

	VOLT_API void shared_lock_all();

	template<typename T>
	void depend();
};

};

#include "system.inl"
