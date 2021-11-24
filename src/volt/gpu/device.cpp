#include <volt/pch.hpp>
#include <volt/gpu/device.hpp>

namespace volt::gpu {

std::shared_ptr<gpu::rasterization_pool> device::create_rasterization_pool() {
	return std::make_shared<gpu::rasterization_pool>(shared_from_this());
}

std::shared_ptr<gpu::compute_pool> device::create_compute_pool() {
	return std::make_shared<gpu::compute_pool>(shared_from_this());
}

std::shared_ptr<gpu::copy_pool> device::create_copy_pool() {
	return std::make_shared<gpu::copy_pool>(shared_from_this());
}

const std::shared_ptr<gpu::adapter> &device::get_adapter() {
	return adapter;
}

device::device(std::shared_ptr<gpu::adapter> &&adapter)
			: adapter(std::move(adapter)) {}

}
