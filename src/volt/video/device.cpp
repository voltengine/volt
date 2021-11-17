#include <volt/pch.hpp>
#include <volt/video/device.hpp>

namespace volt::video {

std::shared_ptr<video::graphics_queue> device::access_graphics_queue() {
	auto cache = graphics_queue.lock();
	if (cache)
		return cache;
	
	auto queue = new_graphics_queue();
	graphics_queue = queue;
	return queue;
}

std::shared_ptr<video::compute_queue> device::access_compute_queue() {
	auto cache = compute_queue.lock();
	if (cache)
		return cache;
	
	auto queue = new_compute_queue();
	compute_queue = queue;
	return queue;
}

std::shared_ptr<video::copy_queue> device::access_copy_queue() {
	auto cache = copy_queue.lock();
	if (cache)
		return cache;
	
	auto queue = new_copy_queue();
	copy_queue = queue;
	return queue;
}

const std::shared_ptr<video::adapter> &device::get_adapter() {
	return adapter;
}

device::device(std::shared_ptr<video::adapter> &&adapter)
			: adapter(std::move(adapter)) {}

}
