#pragma once

#include <volt/pch.hpp>

#include <volt/math/math.hpp>
#include <volt/video/vk12/device.hpp>
#include <volt/video/enums.hpp>
#include <volt/video/texture.hpp>
#include <volt/video/queue.hpp>

namespace volt::video::vk12 {

class texture : public video::texture {
public:
	enum class type {
		tex1d, tex2d, tex3d 
	};

	VkImage image;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VkFormat format;

	texture(std::shared_ptr<video::device> &&device,
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			uint32_t size, uint32_t levels, uint32_t layers,
			video::texture_format format);

	texture(std::shared_ptr<video::device> &&device,
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			video::texture_format format);

	texture(std::shared_ptr<video::device> &&device,
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			video::texture_format format);

	texture(std::shared_ptr<video::device> &&device, VkImage image, VkFormat format);

	~texture();

	std::shared_ptr<video::texture_view> create_view(
			video::texture_view_type type,
			video::texture_view_aspects aspects) override;

	void map(void **ptr) override;

	void unmap() override;

	void read(size_t offset, size_t size) override;

	void write(size_t offset, size_t size) override;

private:
	vk12::device &_device;

	void create(VkImageCreateInfo &&image_info,
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			uint32_t levels, video::texture_format format);
};

}