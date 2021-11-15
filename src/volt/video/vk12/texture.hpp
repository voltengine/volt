#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/math/math.hpp>
#include <volt/video/vk12/device.hpp>
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

	VOLT_API texture(std::shared_ptr<video::device> &&device,
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			uint32_t size, uint32_t levels, uint32_t layers,
			video::texture::format format);

	VOLT_API texture(std::shared_ptr<video::device> &&device,
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			video::texture::format format);

	VOLT_API texture(std::shared_ptr<video::device> &&device,
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			video::texture::format format);

	VOLT_API texture(std::shared_ptr<video::device> &&device, VkImage image, VkFormat format);

	VOLT_API ~texture();

	VOLT_API std::shared_ptr<video::texture_view> create_view(
			video::texture_view::type type,
			video::texture_view::aspects aspects) override;

	VOLT_API void map(void **ptr) override;

	VOLT_API void unmap() override;

	VOLT_API void read(size_t offset, size_t size) override;

	VOLT_API void write(size_t offset, size_t size) override;

private:
	vk12::device &_device;

	void create(VkImageCreateInfo &&image_info,
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			uint32_t levels, video::texture::format format);
};

}