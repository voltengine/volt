#include <volt/pch.hpp>
#include <volt/gpu/swapchain.hpp>

namespace volt::gpu {

float swapchain::framerate_limit() {
	return 1 / frame_time;
}

void swapchain::limit_framerate(float fps) {
	frame_time = 1 / fps;
}

gpu::present_mode swapchain::present_mode() {
	return _present_mode;
}

void swapchain::request_present_mode(gpu::present_mode mode) {
	_present_mode = mode;
	reconstruct(); // Adjusts present_mode
}

const std::shared_ptr<gpu::device> &swapchain::device() {
	return _device;
}

const std::shared_ptr<os::window> &swapchain::window() {
	return _window;
}

swapchain::swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
		: _device(std::move(device)), _window(std::move(window)) {
	framerate_timer.start();
}

}
