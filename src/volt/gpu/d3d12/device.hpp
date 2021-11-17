#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/device.hpp>

namespace volt::gpu::d3d12 {

class device : public gpu::device {
public:
	ID3D12Device *d3d_device;

#ifdef VOLT_GPU_DEBUG
	ID3D12DebugDevice* debug_device;
#endif

	device(std::shared_ptr<gpu::adapter> &&adapter);

	~device();

	void wait() override;

	std::shared_ptr<gpu::buffer> create_buffer(
			gpu::resource_type resource_type,
			gpu::sync_queues sync_queues,
			gpu::buffer_features features,
			size_t size) override;

	std::shared_ptr<gpu::fence> create_fence(uint64_t initial_value = 0) override;

	std::shared_ptr<gpu::surface> create_surface(std::shared_ptr<os::window> window) override;

	std::shared_ptr<gpu::texture> create_texture(
			gpu::resource_type resource_type,
			gpu::sync_queues sync_queues,
			gpu::texture_features features,
			size_t size, uint32_t levels, uint32_t layers,
			gpu::texture_format format) override;

	std::shared_ptr<gpu::texture> create_texture(
			gpu::resource_type resource_type,
			gpu::sync_queues sync_queues,
			gpu::texture_features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			gpu::texture_format format) override;

	std::shared_ptr<gpu::texture> create_texture(
			gpu::resource_type resource_type,
			gpu::sync_queues sync_queues,
			gpu::texture_features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			gpu::texture_format format) override;

private:
	std::shared_ptr<gpu::graphics_queue> new_graphics_queue() override;

	std::shared_ptr<gpu::compute_queue> new_compute_queue() override;

	std::shared_ptr<gpu::copy_queue> new_copy_queue() override;
};


}
