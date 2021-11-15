#pragma once

#include "../macros.hpp"

#include <memory>

#include "routine.hpp"

namespace volt::video {

class queue;

class pool {
public:
	virtual std::shared_ptr<video::routine> &create_routine() = 0;

	const std::shared_ptr<video::queue> &get_queue() {
		return queue;
	}

protected:
	std::shared_ptr<video::queue> queue;

	pool(std::shared_ptr<video::queue> &&queue)
			: queue(std::move(queue)) {}
};

}
