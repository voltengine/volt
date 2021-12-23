#include <volt/gpu/vk12/texture.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

static std::set<gpu::texture_format> depth_formats{
	gpu::texture_format::d16,
	gpu::texture_format::d32f,
	gpu::texture_format::d16_s8,
	gpu::texture_format::d32f_s8,
	gpu::texture_format::d24_s8
};

static std::set<gpu::texture_format> stencil_formats{
	gpu::texture_format::s8,
	gpu::texture_format::d16_s8,
	gpu::texture_format::d32f_s8,
	gpu::texture_format::d24_s8
};

static std::unordered_map<vk12::texture::type, VkImageType> image_types{
	{ vk12::texture::type::tex1d, VK_IMAGE_TYPE_1D },
	{ vk12::texture::type::tex2d, VK_IMAGE_TYPE_2D },
	{ vk12::texture::type::tex3d, VK_IMAGE_TYPE_3D },
	{ vk12::texture::type::cube, VK_IMAGE_TYPE_2D },
	{ vk12::texture::type::tex1d_array, VK_IMAGE_TYPE_1D },
	{ vk12::texture::type::tex2d_array, VK_IMAGE_TYPE_2D },
	{ vk12::texture::type::cube_array, VK_IMAGE_TYPE_2D }
};

static std::set<vk12::texture::type> array_types{
	vk12::texture::type::tex1d_array,
	vk12::texture::type::tex2d_array,
	vk12::texture::type::cube_array,
};

static std::unordered_map<vk12::texture::type, VkImageViewType> view_types{
	{ vk12::texture::type::tex1d, VK_IMAGE_VIEW_TYPE_1D },
	{ vk12::texture::type::tex2d, VK_IMAGE_VIEW_TYPE_2D },
	{ vk12::texture::type::tex3d, VK_IMAGE_VIEW_TYPE_3D },
	{ vk12::texture::type::cube, VK_IMAGE_VIEW_TYPE_CUBE },
	{ vk12::texture::type::tex1d_array, VK_IMAGE_VIEW_TYPE_1D_ARRAY },
	{ vk12::texture::type::tex2d_array, VK_IMAGE_VIEW_TYPE_2D_ARRAY },
	{ vk12::texture::type::cube_array, VK_IMAGE_VIEW_TYPE_CUBE_ARRAY }
};

texture::texture(std::shared_ptr<gpu::device> &&device,
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uint32_t levels, math::uvec3 size, type type)
		: gpu::texture(std::move(device), format, levels, size),
		vk12_device(*static_cast<vk12::device *>(this->_device.get())) {
	auto &adapter = *static_cast<vk12::adapter *>(_device->adapter().get());
	
	aspect_mask = 0;
	if (depth_formats.contains(format)) {
		if (stencil_formats.contains(format))
			aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		else
			aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
	} else if (stencil_formats.contains(format))
		aspect_mask = VK_IMAGE_ASPECT_STENCIL_BIT;
	else
		aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkFormat vk_format = vk12::texture_formats[format];
	VkImageUsageFlags usage = 0;
	if (features & texture_feature::copy_src)
		usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (features & texture_feature::copy_dst)
		usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	if (features & texture_feature::sampled)
		usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
	if (features & texture_feature::storage)
		usage |= VK_IMAGE_USAGE_STORAGE_BIT;
	if (features & texture_feature::attachment)
		usage |= aspect_mask == VK_IMAGE_ASPECT_COLOR_BIT ? VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT : VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

	bool is_array = array_types.contains(type);

	VkImageCreateInfo image_info{};
	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.imageType = image_types[type];
	image_info.format = vk_format;
	image_info.extent.width = size.x;
	image_info.extent.height = size.y;
	image_info.extent.depth = is_array ? 1 : size.z;
	image_info.mipLevels = levels;
	image_info.arrayLayers = is_array ? size.z : 1;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = usage;
	image_info.sharingMode = adapter.unique_families.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
	image_info.queueFamilyIndexCount = adapter.unique_families.size();
	image_info.pQueueFamilyIndices = adapter.unique_families.data();
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	VmaAllocationCreateInfo allocation_info = {};
	allocation_info.usage = vk12::vma_memory_usages[memory_type];

	VOLT_VK12_CHECK(vmaCreateImage(vk12_device.allocator, &image_info, &allocation_info, &image, &allocation, nullptr),
			"Failed to create image.")

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = view_types[type];
	view_info.format = vk_format;
	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.subresourceRange.aspectMask = aspect_mask;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VOLT_VK12_CHECK(vkCreateImageView(vk12_device.vk_device, &view_info, nullptr, &image_view),
			"Failed to create image view.")

	// if (!(features & texture_feature::sampled))
	// 	return;

	// // TODO: Configurable wrapping, filtering and anisotropy
	// VkSamplerCreateInfo sampler_info{};
	// sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	// sampler_info.magFilter = VK_FILTER_LINEAR;
	// sampler_info.minFilter = VK_FILTER_LINEAR;
	// sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	// sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	// sampler_info.mipLodBias = 0;
	// sampler_info.anisotropyEnable = VK_TRUE;
	// sampler_info.maxAnisotropy = adapter.physical_device_properties.limits.maxSamplerAnisotropy;
	// sampler_info.compareEnable = VK_FALSE;
	// sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
	// sampler_info.minLod = 0;
	// sampler_info.maxLod = VK_LOD_CLAMP_NONE;
	// sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	// sampler_info.unnormalizedCoordinates = VK_FALSE;

	// VOLT_VK12_CHECK(vkCreateSampler(vk12_device.vk_device, &sampler_info, nullptr, &sampler),
	// 		"Failed to create image view.")
}

texture::texture(std::shared_ptr<gpu::device> &&device, gpu::texture_format format, math::uvec2 size, VkImage image) :
		gpu::texture(std::move(device), format, 1, 1),
		vk12_device(*static_cast<vk12::device *>(this->_device.get())),
		image(image) {
	aspect_mask = 0;
	if (depth_formats.contains(format)) {
		if (stencil_formats.contains(format))
			aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
		else
			aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT;
	} else if (stencil_formats.contains(format))
		aspect_mask = VK_IMAGE_ASPECT_STENCIL_BIT;
	else
		aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;

	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = vk12::texture_formats[format];
	view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_info.subresourceRange.aspectMask = aspect_mask;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	VOLT_VK12_CHECK(vkCreateImageView(vk12_device.vk_device, &view_info, nullptr, &image_view),
			"Failed to create image view.")

	descriptor_info.sampler = 0; // get from cache or create based on image requirements
	descriptor_info.imageView = image_view;
	descriptor_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}

texture::~texture() {
	// if (sampler != VK_NULL_HANDLE)
	// 	vkDestroySampler(vk12_device.vk_device, sampler, nullptr);
	vkDestroyImageView(vk12_device.vk_device, image_view, nullptr);
	if (allocation != VK_NULL_HANDLE)
		vmaDestroyImage(vk12_device.allocator, image, allocation);
}

void texture::map(void **ptr, size_t read_offset, size_t read_size) {
	VOLT_VK12_DEBUG_CHECK(vmaMapMemory(vk12_device.allocator, allocation, ptr),
			"Failed to map memory.");
	
	if (read_size != 0)
		VOLT_VK12_DEBUG_CHECK(vmaInvalidateAllocation(vk12_device.allocator, allocation, read_offset, read_size),
				"Failed to invalidate allocation.");
}

void texture::unmap(size_t write_offset, size_t write_size) {
	vmaUnmapMemory(vk12_device.allocator, allocation);

	if (write_size != 0)
		VOLT_VK12_DEBUG_CHECK(vmaFlushAllocation(vk12_device.allocator, allocation, write_offset, write_size),
				"Failed to flush allocation.");
}

void texture::barrier(VkCommandBuffer command_buffer, texture::state state) {
	access_pattern access_pattern = access_patterns[state];

	// Some access types do not need synchronization and transitions
	if ((state == texture::state::copy_src && current_state == texture::state::copy_src) || // copy source -> copy source
			(state == texture::state::present && current_state == texture::state::present) || // present -> present
			((state == texture::state::rasterization_shared || state == texture::state::compute_shared) && // shader shared -> shader shared
			(current_state == texture::state::rasterization_shared || current_state == texture::state::compute_shared))) {
		current_access_pattern.stage_mask |= access_pattern.stage_mask;
		current_access_pattern.access_mask |= access_pattern.access_mask;
		return;
	}
	
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = current_access_pattern.access_mask;
	barrier.dstAccessMask = access_pattern.access_mask;
	barrier.oldLayout = current_access_pattern.layout;
	barrier.newLayout = access_pattern.layout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = aspect_mask;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	vkCmdPipelineBarrier(command_buffer,
    		current_access_pattern.stage_mask, access_pattern.stage_mask,
    		0, 0, nullptr, 0, nullptr, 1, &barrier);

	current_access_pattern = access_pattern;
}

std::unordered_map<vk12::texture::state, texture::access_pattern> texture::access_patterns{
	{ vk12::texture::state::undefined, { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_MEMORY_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED } },
	{ vk12::texture::state::copy_src, { VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL } },
	{ vk12::texture::state::copy_dst, { VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL } },
	{ vk12::texture::state::present, { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, 0, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR } },
	{ vk12::texture::state::color_attachment, { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL } }, // No input attachments, thus no read access
	{ vk12::texture::state::depth_stencil_attachment, { VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT, VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL } },
	{ vk12::texture::state::rasterization_shared, { VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } },
	{ vk12::texture::state::rasterization_unique, { VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL } },
	{ vk12::texture::state::compute_shared, { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL } },
	{ vk12::texture::state::compute_unique, { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT, VK_IMAGE_LAYOUT_GENERAL } }
};

}
