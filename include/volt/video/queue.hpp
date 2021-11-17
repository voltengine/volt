#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"
#include "fence.hpp"
#include "pool.hpp"

namespace volt::video {

class device;

template<command_type T>
class queue : public std::enable_shared_from_this<queue<T>> {
public:
	virtual std::shared_ptr<video::pool<T>> create_pool() = 0;

	virtual void wait(const std::shared_ptr<video::fence> &fence, uint64_t value) = 0;

	virtual void execute(const std::shared_ptr<video::routine<T>> &routine) = 0;

	virtual void signal(const std::shared_ptr<video::fence> &fence, uint64_t value) = 0;

	virtual void submit() = 0;

	virtual void wait() = 0;

	const std::shared_ptr<video::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<video::device> device;

	queue(std::shared_ptr<video::device> &&device)
			: device(std::move(device)) {}
};

using graphics_queue = queue<command_type::graphics>;
using compute_queue = queue<command_type::compute>;
using copy_queue = queue<command_type::copy>;

}
