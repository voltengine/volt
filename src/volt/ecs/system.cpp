#include <volt/pch.hpp>
#include <volt/ecs/system.hpp>

namespace volt::ecs {

void system::unique_lock_all() {
	_unique_mask.set();
}

void system::shared_lock_all() {
	_shared_mask.set();
}

}
