#include <volt/gpu/vk12/surface.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

static void frame_resize_callback() {
	
}

namespace volt::gpu::vk12 {

using namespace math;

surface::surface(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
	: gpu::surface(std::move(device), std::move(window)) {
	auto &_device = *static_cast<vk12::device *>(this->device.get());
	auto &adapter = *static_cast<vk12::adapter *>(_device.get_adapter().get());
	auto &instance = *static_cast<vk12::instance *>(adapter.get_instance().get());

	this->window->_construct_surface();
	
	// Surface
	VOLT_VK12_CHECK(glfwCreateWindowSurface(instance.vk_instance, this->window->_glfw_window, nullptr, &vk_surface),
			"Failed to create window surface.")

	// Swapchain
	VkBool32 present_support;
	vkGetPhysicalDeviceSurfaceSupportKHR(adapter.physical_device,
			adapter.present_family, vk_surface, &present_support);
	VOLT_ASSERT(present_support, "Adapter does not support presentation to chosen surface.")

	VkSurfaceFormatKHR surface_format = adapter.surface_formats[0];
	for (VkSurfaceFormatKHR format : adapter.surface_formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			surface_format = format;
			break;
		}
	}

	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
	for (VkPresentModeKHR mode : adapter.surface_present_modes) {
		if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
			present_mode = mode;
			break;
		}
	}

	VkExtent2D extent = adapter.surface_capabilities.currentExtent;
	if (adapter.surface_capabilities.currentExtent.width
			== std::numeric_limits<uint32_t>::max()) {
		uvec2 frame_size = window->get_frame_size();	
		extent = { frame_size.x, frame_size.y };

		const VkExtent2D &min_extent = adapter.surface_capabilities.minImageExtent;
		const VkExtent2D &max_extent = adapter.surface_capabilities.maxImageExtent;
		extent.width = std::clamp(extent.width, min_extent.width, max_extent.width);
		extent.height = std::clamp(extent.height, min_extent.height, max_extent.height);
	}

	// Require double buffering
	uint32_t image_count = math::max(adapter.surface_capabilities.minImageCount, 2);
	if (adapter.surface_capabilities.maxImageCount != 0)
		image_count = math::min(image_count, adapter.surface_capabilities.maxImageCount);

	VkSwapchainCreateInfoKHR swapchain_info{};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface = vk_surface;
	swapchain_info.minImageCount = image_count;
	swapchain_info.imageFormat = surface_format.format;
	swapchain_info.imageColorSpace = surface_format.colorSpace;
	swapchain_info.imageExtent = extent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	// We must enable concurrent mode if present and graphics families are different
	std::array<uint32_t, 2> indices{ adapter.present_family, adapter.graphics_family };
	if (indices[0] != indices[1]) {
		swapchain_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		swapchain_info.queueFamilyIndexCount = indices.size();
		swapchain_info.pQueueFamilyIndices = indices.data();
	} else
		swapchain_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;

	swapchain_info.preTransform = adapter.surface_capabilities.currentTransform; // currentTransform means no additional transform 
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Disable window alpha blending in compositor
	swapchain_info.presentMode = present_mode; // Use VK_PRESENT_MODE_MAILBOX_KHR or fall back to VK_PRESENT_MODE_FIFO_KHR
	swapchain_info.clipped = VK_TRUE; // Something about windows obscuring each other, always set this to VK_TRUE

	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	VOLT_VK12_CHECK(vkCreateSwapchainKHR(_device.vk_device, &swapchain_info, nullptr, &vk_swapchain),
			"Failed to create swapchain.")

	// Get swapchain textures
	uint32_t num_images;
	vkGetSwapchainImagesKHR(_device.vk_device, vk_swapchain, &num_images, nullptr);
	std::vector<VkImage> images(num_images);
	vkGetSwapchainImagesKHR(_device.vk_device, vk_swapchain, &num_images, images.data());

	// Create textures and views
	// for (VkImage image : images) {
	// 	auto texture = std::make_shared<vk12::texture>(std::shared_ptr(this->device), image, surface_format.format);
	// 	auto view = texture->create_view(gpu::texture_view::type::tex2d, gpu::texture_view::aspect::color);
	// }

	// // Create Framebuffers

	// std::vector<VkFramebuffer> swapchain_framebuffers(swapchain_image_views.size());
	// for (size_t i = 0; i < swapchain_framebuffers.size(); i++) {
	// 	VkFramebufferCreateInfo framebufferInfo{};
	// 	framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	// 	framebufferInfo.renderPass = render_pass;
	// 	framebufferInfo.attachmentCount = 1;
	// 	framebufferInfo.pAttachments = &swapchain_image_views[i];
	// 	framebufferInfo.width = swap_extent.width;
	// 	framebufferInfo.height = swap_extent.height;
	// 	framebufferInfo.layers = 1;

	// 	if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchain_framebuffers[i]) != VK_SUCCESS)
	// 		throw std::runtime_error("Failed to create framebuffer.");
	// }

	this->window->_on_frame_resize(frame_resize_callback);
}

surface::~surface() {
	auto &_device = *static_cast<vk12::device *>(device.get());
	auto &_instance = *static_cast<vk12::instance *>(device->get_adapter()->get_instance().get());

	vkDestroySwapchainKHR(_device.vk_device, vk_swapchain, nullptr);
	vkDestroySurfaceKHR(_instance.vk_instance, vk_surface, nullptr);

	window->_destruct_surface();
}

// std::vector<const std::shared_ptr<gpu::texture>> &surface::get_frames() {
	
// }

}
