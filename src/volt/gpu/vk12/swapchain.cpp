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

swapchain::swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
		: gpu::swapchain(std::move(device), std::move(window)),
		vk12_device(*static_cast<vk12::device *>(this->_device.get())) {
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

	// Routines and semaphores
	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	for (uint32_t i = 0; i < vk12::concurrent_frames; i++) {
		routines[i] = this->_device->create_universal_routine();
		VOLT_VK12_CHECK(vkCreateSemaphore(vk12_device.vk_device, &semaphore_info, nullptr,
				&acquire_semaphores[i]), "Failed to create semaphore.")
	}

	// Swapchain
	create();
}

swapchain::~swapchain() {
	destroy();

	for (uint32_t i = 0; i < vk12::concurrent_frames; i++)
		vkDestroySemaphore(vk12_device.vk_device, acquire_semaphores[i], nullptr);

	VkInstance instance = static_cast<vk12::instance *>(_device->adapter()->instance().get())->vk_instance;
	vkDestroySurfaceKHR(instance, surface, nullptr);
	_window->_destruct_swapchain();
}

void swapchain::next_frame(std::function<void(frame)> &&callback) {
	if (framerate_timer.elapsed() < frame_time)
		return;
	framerate_timer.reset();

	if (math::any(frame_size != _window->get_frame_size())) {
		destroy();
		create();
	}

	if (vk_swapchain == VK_NULL_HANDLE)
		return;
	
	VkSemaphore acquire_semaphore = acquire_semaphores[current_frame];

	uint32_t texture_index;
	VkResult result = vkAcquireNextImageKHR(vk12_device.vk_device, vk_swapchain, 0,
			acquire_semaphore, VK_NULL_HANDLE, &texture_index);
	
	if (result == VK_NOT_READY)
		return;
	else
		VOLT_VK12_DEBUG_CHECK(result, "Failed to acquire image.")

	auto &routine = *static_cast<vk12::universal_routine *>(routines[current_frame].get());
	if (!routine.finished())
		return;

	auto &texture = textures[texture_index];
	auto &vk12_texture = *static_cast<vk12::texture *>(texture.get());

	vk12_texture.swapchain_acquire_semaphore = acquire_semaphore;

	routine.execute([&](universal_routine_context &routine_context) {
		callback(frame{
			.index = current_frame,
			.texture = texture,
			.routine_context = routine_context
		});

		vk12_texture.barrier(routine.impl, vk12::texture::state::present);
	});

	VkPresentInfoKHR present_info = {};
	present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	present_info.swapchainCount = 1;
	present_info.pSwapchains = &vk_swapchain;
	present_info.pImageIndices = &texture_index;

	result = vkQueuePresentKHR(routine.impl.queue, &present_info);
	
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		destroy();
		create();
	} else
		VOLT_VK12_DEBUG_CHECK(result, "Failed to present swapchain image.");
	
	current_frame = (current_frame + 1) % vk12::concurrent_frames;
}

void swapchain::create() {
	auto &adapter = *static_cast<vk12::adapter *>(_device->adapter().get());

	for (VkSurfaceFormatKHR format : adapter.surface_formats) {
		if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			goto format_found;
	}
	VOLT_ASSERT(false, "No suitable surface format found.")
format_found:

	// The only guaranteed mode
	VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
	for (VkPresentModeKHR mode : adapter.surface_present_modes) {
		if (_present_mode == gpu::present_mode::tear) {
			if ((_present_mode == gpu::present_mode::tear && mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
					|| (_present_mode == gpu::present_mode::triple_buffer && mode == VK_PRESENT_MODE_MAILBOX_KHR)) {
				present_mode = mode;
				break;
			}
		}
	}

	VkExtent2D extent;

	VkSurfaceCapabilitiesKHR surface_capabilities;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter.physical_device, surface, &surface_capabilities);

	if (surface_capabilities.currentExtent.width == 0xFFFFFFFF) {
		frame_size = _window->get_frame_size();
		const VkExtent2D &min_extent = surface_capabilities.minImageExtent;
		const VkExtent2D &max_extent = surface_capabilities.maxImageExtent;

		extent.width = math::clamp(frame_size.x, min_extent.width, max_extent.width);
		extent.height = math::clamp(frame_size.y, min_extent.height, max_extent.height);
	} else {
		extent.width = frame_size.x = surface_capabilities.currentExtent.width,
		extent.height = frame_size.y = surface_capabilities.currentExtent.height;
	}


	if (extent.width == 0 || extent.height == 0)
		return; // Swapchain becomes disabled

	uint32_t image_count = _present_mode == gpu::present_mode::triple_buffer ? 3 : 2;
	image_count = math::max(image_count, surface_capabilities.minImageCount);
	if (surface_capabilities.maxImageCount != 0)
		image_count = math::min(image_count, surface_capabilities.maxImageCount);

	VOLT_ASSERT(image_count >= 2, "Swapchain requires double buffering.");
	
	// Fall back from triple buffering to v-sync
	if ((_present_mode == gpu::present_mode::triple_buffer && (image_count < 3 || present_mode != VK_PRESENT_MODE_MAILBOX_KHR))
			|| (_present_mode == gpu::present_mode::tear && present_mode != VK_PRESENT_MODE_IMMEDIATE_KHR))
		_present_mode = gpu::present_mode::vsync;

	VkSwapchainCreateInfoKHR swapchain_info{};
	swapchain_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapchain_info.surface = surface;
	swapchain_info.minImageCount = image_count; // Double-buffering forces 30 fps as soon as gpu goes over 16 ms, triple buffering requires more memory
	swapchain_info.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
	swapchain_info.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
	swapchain_info.imageExtent = extent;
	swapchain_info.imageArrayLayers = 1;
	swapchain_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapchain_info.imageSharingMode = adapter.unique_families.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
	swapchain_info.queueFamilyIndexCount = adapter.unique_families.size();
	swapchain_info.pQueueFamilyIndices = adapter.unique_families.data();
	swapchain_info.preTransform = surface_capabilities.currentTransform; // currentTransform means no additional transform 
	swapchain_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // Disable window alpha blending in compositor
	swapchain_info.presentMode = present_mode; // Use VK_PRESENT_MODE_MAILBOX_KHR or fall back to VK_PRESENT_MODE_FIFO_KHR which can cause input lag
	swapchain_info.clipped = VK_TRUE; // Something about windows obscuring each other, always set this to VK_TRUE
	swapchain_info.oldSwapchain = VK_NULL_HANDLE;

	VOLT_VK12_CHECK(vkCreateSwapchainKHR(vk12_device.vk_device, &swapchain_info, nullptr, &vk_swapchain),
			"Failed to create swapchain.")

	// Get swapchain textures
	uint32_t num_images;
	vkGetSwapchainImagesKHR(vk12_device.vk_device, vk_swapchain, &num_images, nullptr);
	std::vector<VkImage> images(num_images);
	vkGetSwapchainImagesKHR(vk12_device.vk_device, vk_swapchain, &num_images, images.data());

	// Create textures
	for (VkImage image : images)
		textures.emplace_back(new vk12::texture(std::shared_ptr(this->_device),
				gpu::texture_format::present, math::uvec2(extent.width, extent.height), image));
}

void swapchain::destroy() {
	if (vk_swapchain == VK_NULL_HANDLE)
		return;

	vk12_device.wait();
	vk12_device.jit.clear_framebuffers();

	textures.clear();
	vkDestroySwapchainKHR(vk12_device.vk_device, vk_swapchain, nullptr);
	vk_swapchain = VK_NULL_HANDLE;
}

}
