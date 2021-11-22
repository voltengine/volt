#pragma once

#include <volt/pch.hpp>

#include <volt/math/math.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/enums.hpp>
#include <volt/gpu/texture.hpp>
#include <volt/gpu/queue.hpp>

namespace volt::gpu::vk12 {

class texture : public gpu::texture {
public:
	enum class type {
		tex1d, tex2d, tex3d 
	};

	VkImage image;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VkFormat format;

	texture(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			uint32_t size, uint32_t levels,
			gpu::texture_format format);

	texture(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			uint32_t size, uint32_t layers, uint32_t levels,
			gpu::texture_format format);

	texture(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			math::uvec2 size, uint32_t layers, uint32_t levels,
			gpu::texture_format format);

	texture(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			math::uvec3 size, uint32_t levels,
			gpu::texture_format format);

	texture(std::shared_ptr<gpu::device> &&device, VkImage image, VkFormat format);

	~texture();

	std::shared_ptr<gpu::texture_view> create_view(
			gpu::texture_view_type type,
			gpu::texture_view_aspects aspects) override;

	void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) override;

	void unmap(size_t write_offset = 0, size_t write_size = 0) override;

private:
	vk12::device &_device;

	void create(VkImageCreateInfo &&image_info,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			uint32_t levels, gpu::texture_format format);
};

}