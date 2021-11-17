#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"
#include "routine.hpp"

namespace volt::video {

template<command_type T>
class queue;

template<command_type T>
class pool : public std::enable_shared_from_this<pool<T>> {
public:
	virtual std::shared_ptr<video::routine<T>> create_routine() = 0;

	virtual std::shared_ptr<video::subroutine<T>> create_subroutine() = 0;

	virtual void reset_routines() = 0;

	const std::shared_ptr<video::queue<T>> &get_queue() {
		return queue;
	}

protected:
	std::shared_ptr<video::queue<T>> queue;

	pool(std::shared_ptr<video::queue<T>> &&queue)
			: queue(std::move(queue)) {}
};

using graphics_pool = pool<command_type::graphics>;
using compute_pool = pool<command_type::compute>;
using copy_pool = pool<command_type::copy>;

}
