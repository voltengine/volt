#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/device.hpp>
#include <volt/gpu/vk12/adapter.hpp>

namespace volt::gpu::vk12 {

class device : public gpu::device {
public:
	VkDevice vk_device;
	VmaAllocator allocator;

	device(std::shared_ptr<gpu::adapter> &&adapter);
	
	~device();

	void wait() override;

	std::shared_ptr<gpu::buffer> create_buffer(
			gpu::memory_type memory_type,
			gpu::buffer_features features,
			size_t size) override;

	std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) override;

	std::shared_ptr<gpu::texture> create_2d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			math::uvec2 size, uint32_t levels) override;

	// std::shared_ptr<gpu::pipeline> create_pipeline() override;

private:
	VkQueue vk_rasterization_queue, vk_compute_queue, vk_copy_queue;
};

}
