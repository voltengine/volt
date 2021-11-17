#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/surface.hpp>
#include <volt/gpu/vk12/device.hpp>

namespace volt::gpu::vk12 {

class surface : public gpu::surface {
public:
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;

	surface(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window);

	~surface();

	// virtual std::vector<const std::shared_ptr<gpu::texture>> &get_frames() = 0;
};

}
