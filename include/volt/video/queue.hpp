#pragma once

#include "../macros.hpp"

namespace volt::video {

class queue {
public:
	using types = uint32_t;
	struct type {
		static constexpr types
				graphics = 1 << 0,
				compute  = 1 << 1,
				copy     = 1 << 2;
	};

	virtual void wait() = 0;

	// virtual void submit(routines, signal_fence, wait_semaphores, signal_semaphores)
protected:
	VOLT_API queue() = default;
};

}
