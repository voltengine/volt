#include <volt/gpu/vk12/swapchain.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

static void create_swapchain() {

}

static void frame_resize_callback() {
	
}

swapchain::swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
		: gpu::swapchain(std::move(device), std::move(window)),
		vk_device(static_cast<vk12::device *>(this->_device.get())->vk_device) {
	this->_window->_construct_swapchain();

	auto &adapter = *static_cast<vk12::adapter *>(_device->adapter().get());
	VkInstance vk_instance = static_cast<vk12::instance *>(adapter.instance().get())->vk_instance;
	
	// Surface
	VOLT_VK12_CHECK(glfwCreateWindowSurface(vk_instance, this->_window->_glfw_window, nullptr, &surface),
			"Failed to create window surface.")

	VkBool32 present_support;
	vkGetPhysicalDeviceSurfaceSupportKHR(adapter.physical_device,
			adapter.universal_family, surface, &present_support);
	VOLT_ASSERT(present_support, "Adapter does not support presentation to chosen surface.")

	// Swapchain
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
		frame_size = window->get_frame_size();	
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
	swapchain_info.surface = surface;
	swapchain_info.minImageCount = image_count; // Double-buffering forces 30 fps as soon as gpu goes over 16 ms, triple buffering requires more memory
	swapchain_info.imageFormat = surface_format.format;
	swapchain_info.imageColorSpace = surface_format.colorSpace;
	swapchain_info.imageExtent = extent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_info.imageSharingMode = adapter.unique_families.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
	swapchain_info.queueFamilyIndexCount = adapter.unique_families.size();
	swapchain_info.pQueueFamilyIndices = adapter.unique_families.data();

	swapchain_info.preTransform = adapter.surface_capabilities.currentTransform; // currentTransform means no additional transform 
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Disable window alpha blending in compositor
	swapchain_info.presentMode = present_mode; // Use VK_PRESENT_MODE_MAILBOX_KHR or fall back to VK_PRESENT_MODE_FIFO_KHR which can cause input lag
	swapchain_info.clipped = VK_TRUE; // Something about windows obscuring each other, always set this to VK_TRUE

	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	VOLT_VK12_CHECK(vkCreateSwapchainKHR(vk_device, &swapchain_info, nullptr, &vk_swapchain),
			"Failed to create swapchain.")

	// Get swapchain textures
	uint32_t num_images;
	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &num_images, nullptr);
	std::vector<VkImage> images(num_images);
	vkGetSwapchainImagesKHR(vk_device, vk_swapchain, &num_images, images.data());

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

	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (uint32_t i = 0; i < vk12::concurrent_frames; i++) {
		VOLT_VK12_CHECK(vkCreateSemaphore(vk_device, &semaphore_info, nullptr,
				&frame_begin_semaphores[i]), "Failed to create semaphore.")
		VOLT_VK12_CHECK(vkCreateSemaphore(vk_device, &semaphore_info, nullptr,
				&frame_end_semaphores[i]), "Failed to create semaphore.")
	}
}

swapchain::~swapchain() {
	VkInstance instance = static_cast<vk12::instance *>(_device->adapter()->instance().get())->vk_instance;

	vkDestroySwapchainKHR(vk_device, vk_swapchain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);

	_window->_destruct_swapchain();
}

void swapchain::next_frame(std::function<void(frame)> &&callback) {
	if (framerate_timer.elapsed() < frame_time)
		return;
	framerate_timer.reset();

	if (math::any(frame_size != _window->get_frame_size()))
		reconstruct();

	uint32_t texture_index;
	VkResult result = vkAcquireNextImageKHR(vk_device, vk_swapchain, 0,
			frame_begin_semaphores[current_frame], VK_NULL_HANDLE, &texture_index);
	
	if (result == VK_NOT_READY)
		return;
	else
		VOLT_VK12_DEBUG_CHECK(result, "Failed to acquire image.")

	// transition from present etc.

	auto *routine = static_cast<vk12::universal_routine *>(routines[current_frame].get());
	auto &texture = textures[texture_index];

	routine->execute([&](universal_executor &executor) {
		callback(frame{
			.index = current_frame,
			.texture = texture,
			.executor = executor
		});

		
	});

	static_cast<vk12::texture *>(texture.get())->barrier(routine->impl.command_buffer, vk12::texture::state::present);

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &vk_swapchain;
	present_info.pImageIndices = &texture_index;

	result = vkQueuePresentKHR(routine->impl.queue, &present_info);
	
	// if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || just_resized)
	// 	recreate_swapchain();
	// else
	VOLT_VK12_DEBUG_CHECK(result, "Failed to present swapchain image.");
	
	current_frame = (current_frame + 1) % vk12::concurrent_frames;
}

void swapchain::reconstruct() {
	
}

}
