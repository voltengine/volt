#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/swapchain.hpp>
#include <volt/gpu/vk12/device.hpp>

namespace volt::gpu::vk12 {

class swapchain : public gpu::swapchain {
public:
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;

	swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window);

	~swapchain();

	// virtual std::vector<const std::shared_ptr<gpu::texture>> &get_frames() = 0;
};

}
