#include <volt/pch.hpp>
#include <volt/ecs/system.hpp>

namespace volt::ecs {

async_system::async_system() {
	_async = true;
}

void async_system::unique_lock_all() {
	_unique_mask.set();
}

void async_system::shared_lock_all() {
	_shared_mask.set();
}

}
