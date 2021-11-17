#include <volt/video/d3d12/device.hpp>

#include <volt/math/math.hpp>
#include <volt/video/d3d12/adapter.hpp>
#include <volt/video/d3d12/fence.hpp>
#include <volt/video/d3d12/d3d12.hpp>
#include <volt/error.hpp>

namespace volt::video::d3d12 {

device::device(std::shared_ptr<video::adapter> &&adapter) : video::device(std::move(adapter)) {
	auto d3d_adapter = static_cast<d3d12::adapter *>(this->adapter.get())->d3d_adapter;
	
	VOLT_D3D12_CHECK(D3D12CreateDevice(d3d_adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3d_device)),
			"Failed to create device.")

	VOLT_D3D12_CHECK(d3d_device->QueryInterface(&debug_device),
			"Failed to query device debug.")
	
	// create queues
}

device::~device() {
	d3d_device->Release();
}

void device::wait() {
	// TODO: Wait for each queue
}

std::shared_ptr<video::buffer> device::create_buffer(
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::buffer_features features,
		size_t size) {
	return nullptr;
}

std::shared_ptr<video::fence> device::create_fence(uint64_t initial_value) {
	return std::shared_ptr<video::fence>(new d3d12::fence(
			shared_from_this(), initial_value));
}

std::shared_ptr<video::surface> device::create_surface(std::shared_ptr<os::window> window) {
	return nullptr;
}

std::shared_ptr<video::texture> device::create_texture(
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		size_t size, uint32_t levels, uint32_t layers,
		video::texture_format format) {
	return nullptr;
}

std::shared_ptr<video::texture> device::create_texture(
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		math::uvec2 size, uint32_t levels, uint32_t layers,
		video::texture_format format) {
	return nullptr;
}

std::shared_ptr<video::texture> device::create_texture(
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		math::uvec3 size, uint32_t levels, uint32_t layers,
		video::texture_format format) {
	return nullptr;
}

// std::shared_ptr<video::pipeline> device::create_pipeline() {
// 	return nullptr; // TODO: Implement
// }

std::shared_ptr<video::graphics_queue> device::new_graphics_queue() {
	return nullptr;
}

std::shared_ptr<video::compute_queue> device::new_compute_queue() {
	return nullptr;
}

std::shared_ptr<video::copy_queue> device::new_copy_queue() {
	return nullptr;
}

}
