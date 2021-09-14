#pragma once

#include "../macros.hpp"

#include <bitset>

#include "ecs.hpp"

namespace volt::ecs {

class system {
public:
	std::bitset<VOLT_MAX_COMPONENTS> _unique_mask, _shared_mask;

	virtual ~system() = default;

	virtual void update(float delta) const = 0;

protected:
	template<typename T>
	void unique_lock();

	template<typename T>
	void shared_lock();

	VOLT_API void unique_lock_all();

	VOLT_API void shared_lock_all();
};

};

#include "system.inl"
