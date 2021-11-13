#include <volt/pch.hpp>
#include <volt/video/vk12/swapchain.hpp>

#include <volt/error.hpp>

namespace volt::video::vk12::_internal {

using namespace math;

swapchain::swapchain(_internal::device &device, _internal::window &window) : device(device) {
	VkBool32 present_support;
	vkGetPhysicalDeviceSurfaceSupportKHR(device.adapter.vk_physical_device,
			device.adapter.present_family, window.vk_surface, &present_support);
	VOLT_ASSERT(present_support, "Adapter does not support presentation to chosen surface.")

	VkSurfaceFormatKHR surface_format = device.adapter.surface_formats[0];
	for (VkSurfaceFormatKHR format : device.adapter.surface_formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			surface_format = format;
			break;
		}
	}

	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
	for (VkPresentModeKHR mode : device.adapter.surface_present_modes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			present_mode = mode;
			break;
		}
	}

	VkExtent2D extent = device.adapter.surface_capabilities.currentExtent;
	if (device.adapter.surface_capabilities.currentExtent.width
			== std::numeric_limits<uint32_t>::max()) {		
		uvec2 frame_size = window.get_frame_size();
		extent = { frame_size.x, frame_size.y };

		const VkExtent2D &min_extent = device.adapter.surface_capabilities.minImageExtent;
		const VkExtent2D &max_extent = device.adapter.surface_capabilities.maxImageExtent;
		extent.width = std::clamp(extent.width, min_extent.width, max_extent.width);
		extent.height = std::clamp(extent.height, min_extent.height, max_extent.height);
	}

	// Require double buffering
	uint32_t image_count = math::max(device.adapter.surface_capabilities.minImageCount, 2);
	if (device.adapter.surface_capabilities.maxImageCount != 0)
		image_count = math::min(image_count, device.adapter.surface_capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR swapchain_info = {};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface = window.vk_surface;
	swapchain_info.minImageCount = image_count;
	swapchain_info.imageFormat = surface_format.format;
	swapchain_info.imageColorSpace = surface_format.colorSpace;
	swapchain_info.imageExtent = extent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// We must enable concurrent mode if present and graphics families are different
	std::array<uint32_t, 2> indices{ device.adapter.present_family, device.adapter.graphics_family };
	if (indices[0] != indices[1]) {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_info.queueFamilyIndexCount = indices.size();
		swapchain_info.pQueueFamilyIndices = indices.data();
	} else
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	swapchain_info.preTransform = device.adapter.surface_capabilities.currentTransform; // currentTransform means no additional transform 
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Disable window alpha blending in compositor
	swapchain_info.presentMode = present_mode; // Use VK_PRESENT_MODE_MAILBOX_KHR or fall back to VK_PRESENT_MODE_FIFO_KHR
	swapchain_info.clipped = VK_TRUE; // Something about windows obscuring each other, always set this to VK_TRUE

	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	VOLT_ASSERT(vkCreateSwapchainKHR(device.vk_device, &swapchain_info, nullptr, &vk_swapchain)
			== VK_SUCCESS, "Failed to create swapchain.")

	// uint32_t num_images;
	// vkGetSwapchainImagesKHR(device, vk_swapchain, &num_images, nullptr);
	// swapchain_images.resize(num_images);
	// vkGetSwapchainImagesKHR(device, vl+swapchain, &num_images, swapchain_images.data());
}

swapchain::~swapchain() {
	vkDestroySwapchainKHR(device.vk_device, vk_swapchain, nullptr);
}

}
