#pragma once

#include <volt/pch.hpp>

#include <volt/video/device.hpp>
#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/queue.hpp>

namespace volt::video::vk12 {

class device : public video::device {
public:
	VkDevice vk_device;
	VmaAllocator allocator;

	device(std::shared_ptr<video::adapter> &&adapter);
	
	~device();

	void wait() override;

	std::shared_ptr<video::buffer> create_buffer(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::buffer_features features,
			size_t size) override;

	std::shared_ptr<video::fence> create_fence(uint64_t initial_value = 0) override;

	std::shared_ptr<video::surface> create_surface(std::shared_ptr<os::window> window) override;

	std::shared_ptr<video::texture> create_texture(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			size_t size, uint32_t levels, uint32_t layers,
			video::texture_format format) override;

	std::shared_ptr<video::texture> create_texture(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			video::texture_format format) override;

	std::shared_ptr<video::texture> create_texture(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			video::texture_format format) override;

	// std::shared_ptr<video::pipeline> create_pipeline() override;


private:
	VkQueue vk_graphics_queue, vk_compute_queue, vk_copy_queue;

	std::shared_ptr<video::graphics_queue> new_graphics_queue() override;

	std::shared_ptr<video::compute_queue> new_compute_queue() override;

	std::shared_ptr<video::copy_queue> new_copy_queue() override;
};

}
