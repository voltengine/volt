#include <volt/video/vk12/texture_view.hpp>

#include <volt/video/vk12/device.hpp>
#include <volt/video/vk12/texture.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12 {

static std::map<video::texture_view_type, VkImageViewType> view_types{
	{ video::texture_view_type::tex1d,       VK_IMAGE_VIEW_TYPE_1D },
	{ video::texture_view_type::tex1d_array, VK_IMAGE_VIEW_TYPE_1D_ARRAY },

	{ video::texture_view_type::tex2d,       VK_IMAGE_VIEW_TYPE_2D },
	{ video::texture_view_type::tex2d_array, VK_IMAGE_VIEW_TYPE_2D_ARRAY },

	{ video::texture_view_type::tex3d,       VK_IMAGE_VIEW_TYPE_3D },

	{ video::texture_view_type::cube,        VK_IMAGE_VIEW_TYPE_CUBE },
	{ video::texture_view_type::cube_array,  VK_IMAGE_VIEW_TYPE_CUBE_ARRAY }
};

texture_view::texture_view(
		std::shared_ptr<video::texture> &&texture,
		video::texture_view_type type,
		video::texture_view_aspects aspects)
		: video::texture_view(std::move(texture)) {
	auto &_texture = *static_cast<vk12::texture *>(texture.get());
	auto &device = *static_cast<vk12::device *>(_texture.get_device().get());

	VkImageViewCreateInfo view_create_info{};
	view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_create_info.image = _texture.image;
	view_create_info.viewType = view_types[type];
	view_create_info.format = _texture.format;
	view_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
	view_create_info.subresourceRange.aspectMask = aspects & 0b111;
	view_create_info.subresourceRange.baseMipLevel = 0;
	view_create_info.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
	view_create_info.subresourceRange.baseArrayLayer = 0;
	view_create_info.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;

	VOLT_VK12_CHECK(vkCreateImageView(device.vk_device, &view_create_info, nullptr, &image_view),
			"Failed to create image view.")
}

texture_view::~texture_view() {
	auto &device = *static_cast<vk12::device *>(texture->get_device().get());
	vkDestroyImageView(device.vk_device, image_view, nullptr);
}

}