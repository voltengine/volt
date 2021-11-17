#pragma once

#include "../macros.hpp"

#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device;

class surface : public std::enable_shared_from_this<surface> {
public:
	// virtual std::vector<const std::shared_ptr<gpu::texture>> &get_frames() = 0;

	// virtual void present(swapchains, frame_indices, wait_semaphores) = 0;

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

	const std::shared_ptr<os::window> &get_window() {
		return window;
	}

protected:
	std::shared_ptr<gpu::device> device;
	std::shared_ptr<os::window> window;

	surface(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
			: device(std::move(device)), window(std::move(window)) {}
};

}
