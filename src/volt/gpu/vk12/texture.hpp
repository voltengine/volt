#pragma once

#include <volt/pch.hpp>

#include <volt/math/math.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/enums.hpp>
#include <volt/gpu/texture.hpp>

namespace volt::gpu::vk12 {

class texture : public gpu::texture {
public:
	enum class type {
		tex1d, tex2d, tex3d 
	};

	VkImage image;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VkImageView image_view;

	texture(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			math::uvec2 size, uint32_t levels);

	// Just for swapchain images
	texture(std::shared_ptr<gpu::device> &&device, gpu::texture_format format, VkImage image, VkImageView image_view);

	~texture();

	void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) override;

	void unmap(size_t write_offset = 0, size_t write_size = 0) override;

private:
	vk12::device &_device;
};

}