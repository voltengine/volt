#include <volt/gpu/vk12/texture.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

static std::set<gpu::texture_format> depth_formats {
	gpu::texture_format::d16,
	gpu::texture_format::d32f,
	gpu::texture_format::d16_s8,
	gpu::texture_format::d32f_s8,
	gpu::texture_format::d24_s8
};

static std::set<gpu::texture_format> stencil_formats {
	gpu::texture_format::s8,
	gpu::texture_format::d16_s8,
	gpu::texture_format::d32f_s8,
	gpu::texture_format::d24_s8
};

texture::texture(std::shared_ptr<gpu::device> &&device,
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uvec2 size, uint32_t levels)
		: gpu::texture(std::move(device), format),
		_device(*static_cast<vk12::device *>(this->device.get())) {
	auto &adapter = *static_cast<vk12::adapter *>(_device.get_adapter().get());
	
	uint32_t families[]{ adapter.graphics_family, adapter.compute_family, adapter.transfer_family };
	VkFormat vk_format = vk12::texture_formats[format];

	VkImageCreateInfo image_info{};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.format = vk_format;
	image_info.extent.width = size.x;
	image_info.extent.height = size.y;
	image_info.extent.depth = 1;
	image_info.mipLevels = levels;
	image_info.arrayLayers = 1;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = features & 0b111111;
	image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	image_info.queueFamilyIndexCount = 3;
	image_info.pQueueFamilyIndices = families;
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	VmaAllocationCreateInfo allocation_info = {};
	allocation_info.usage = vk12::vma_memory_usages[memory_type];

	VOLT_VK12_CHECK(vmaCreateImage(_device.allocator, &image_info, &allocation_info, &image, &allocation, nullptr),
			"Failed to create image.")

	VkImageAspectFlags aspect_mask = 0;
	if (depth_formats.contains(format)) {
		if (stencil_formats.contains(format))
			aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		else
			aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
	} else if (stencil_formats.contains(format))
		aspect_mask = VK_IMAGE_ASPECT_STENCIL_BIT;
	else
		aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageViewCreateInfo view_create_info = {};
	view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_create_info.image = image;
	view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_create_info.format = vk_format;
	view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.subresourceRange.aspectMask = aspect_mask;
	view_create_info.subresourceRange.baseMipLevel = 0;
	view_create_info.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	view_create_info.subresourceRange.baseArrayLayer = 0;
	view_create_info.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VOLT_VK12_CHECK(vkCreateImageView(_device.vk_device, &view_create_info, nullptr, &image_view),
			"Failed to create image view.")
}


texture::texture(std::shared_ptr<gpu::device> &&device, gpu::texture_format format, VkImage image, VkImageView image_view) :
		gpu::texture(std::move(device), format),
		_device(*static_cast<vk12::device *>(this->device.get())),
		image(image), image_view(image_view) {}

texture::~texture() {
	if (allocation != VK_NULL_HANDLE) {
		vkDestroyImageView(_device.vk_device, image_view, nullptr);
		vmaDestroyImage(_device.allocator, image, allocation);
	}
}

void texture::map(void **ptr, size_t read_offset, size_t read_size) {
	VOLT_VK12_DEBUG_CHECK(vmaMapMemory(_device.allocator, allocation, ptr),
			"Failed to map memory.");
	
	if (read_size != 0)
		VOLT_VK12_DEBUG_CHECK(vmaInvalidateAllocation(_device.allocator, allocation, read_offset, read_size),
				"Failed to invalidate allocation.");
}

void texture::unmap(size_t write_offset, size_t write_size) {
	vmaUnmapMemory(_device.allocator, allocation);

	if (write_size != 0)
		VOLT_VK12_DEBUG_CHECK(vmaFlushAllocation(_device.allocator, allocation, write_offset, write_size),
				"Failed to flush allocation.");
}

}
