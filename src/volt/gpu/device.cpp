#include <volt/pch.hpp>
#include <volt/gpu/device.hpp>

namespace volt::gpu {

std::shared_ptr<gpu::rasterization_queue> device::get_rasterization_queue() {
	auto cache = rasterization_queue.lock();
	if (cache)
		return cache;
	
	auto queue = new_rasterization_queue();
	rasterization_queue = queue;
	return queue;
}

std::shared_ptr<gpu::compute_queue> device::get_compute_queue() {
	auto cache = compute_queue.lock();
	if (cache)
		return cache;
	
	auto queue = new_compute_queue();
	compute_queue = queue;
	return queue;
}

std::shared_ptr<gpu::copy_queue> device::get_copy_queue() {
	auto cache = copy_queue.lock();
	if (cache)
		return cache;
	
	auto queue = new_copy_queue();
	copy_queue = queue;
	return queue;
}

const std::shared_ptr<gpu::adapter> &device::get_adapter() {
	return adapter;
}

device::device(std::shared_ptr<gpu::adapter> &&adapter)
			: adapter(std::move(adapter)) {}

}
