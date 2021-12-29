#include <volt/pch.hpp>
#include <volt/gpu/swapchain.hpp>

namespace volt::gpu {

float swapchain::framerate_limit() const {
	if (frame_time == 0)
		return 0;
	else
		return 1 / frame_time;
}

void swapchain::framerate_limit(float fps) {
	if (fps == 0)
		frame_time = 0;
	else
		frame_time = 1 / fps;
}

gpu::present_mode swapchain::present_mode() const {
	return _present_mode;
}

void swapchain::present_mode(gpu::present_mode mode) {
	_present_mode = mode;
	destroy();
	create();
}

const std::shared_ptr<gpu::device> &swapchain::device() const {
	return _device;
}

const std::shared_ptr<os::window> &swapchain::window() const {
	return _window;
}

swapchain::swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
		: _device(std::move(device)), _window(std::move(window)) {
	framerate_timer.start();
}

}
