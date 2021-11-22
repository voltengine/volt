#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/device.hpp>
#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/queue.hpp>

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
			gpu::command_types sync_queues,
			gpu::buffer_features features,
			size_t size) override;

	std::shared_ptr<gpu::fence> create_fence(uint64_t initial_value = 0) override;

	std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) override;

	std::shared_ptr<gpu::texture> create_texture(
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			size_t size, uint32_t levels,
			gpu::texture_format format) override;

	std::shared_ptr<gpu::texture> create_texture(
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			math::uvec2 size, uint32_t levels,
			gpu::texture_format format) override;

	std::shared_ptr<gpu::texture> create_texture(
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			math::uvec3 size, uint32_t levels,
			gpu::texture_format format) override;

	// std::shared_ptr<gpu::pipeline> create_pipeline() override;

private:
	VkQueue vk_rasterization_queue, vk_compute_queue, vk_copy_queue;

	std::shared_ptr<gpu::rasterization_queue> new_rasterization_queue() override;

	std::shared_ptr<gpu::compute_queue> new_compute_queue() override;

	std::shared_ptr<gpu::copy_queue> new_copy_queue() override;
};

}
