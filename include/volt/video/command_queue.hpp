#pragma once

#include "../macros.hpp"

namespace volt::video {

class command_queue {
public:
	virtual void await_idle();
	virtual void submit(command_buffers, signal_fence, wait_semaphores, signal_semaphores)
	virtual void present(swapchains, frame_indices, wait_semaphores);
	
	virtual bool present_capable()
	virtual bool graphics_capable()
	virtual bool compute_capable()
	virtual bool transfer_capable()
protected:
	VOLT_API command_queue() = default;
};

}
