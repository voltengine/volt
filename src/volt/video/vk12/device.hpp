#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/device.hpp>
#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/queue.hpp>

namespace volt::video::vk12 {

class device : public video::device {
public:
	VkDevice vk_device;
	VmaAllocator allocator;
	vk12::queue graphics_queue, compute_queue, transfer_queue;

	VOLT_API device(std::shared_ptr<video::adapter> &&adapter);
	
	VOLT_API ~device();

	VOLT_API void wait() override;

	VOLT_API video::queue &get_graphics_queue() override;

	VOLT_API video::queue &get_compute_queue() override;

	VOLT_API video::queue &get_copy_queue() override;

	VOLT_API std::shared_ptr<video::buffer> create_buffer(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::buffer::features features,
			size_t size) override;

	VOLT_API std::shared_ptr<video::surface> device::create_surface(std::shared_ptr<os::window> window) override;

	VOLT_API std::shared_ptr<video::texture> create_texture(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			size_t size, uint32_t levels, uint32_t layers,
			video::texture::format format) override;

	VOLT_API std::shared_ptr<video::texture> create_texture(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			video::texture::format format) override;

	VOLT_API std::shared_ptr<video::texture> create_texture(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			video::texture::format format) override;

	VOLT_API std::shared_ptr<video::shader> create_shader(const std::vector<uint8_t> &bytecode) override;

	// VOLT_API std::shared_ptr<video::pipeline> create_pipeline() override;
};

}
