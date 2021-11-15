#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/surface.hpp>
#include <volt/video/vk12/device.hpp>

namespace volt::video::vk12 {

class surface : public video::surface {
public:
	VkSurfaceKHR vk_surface;
	VkSwapchainKHR vk_swapchain;

	VOLT_API surface(std::shared_ptr<video::device> &&device, std::shared_ptr<os::window> &&window);

	VOLT_API ~surface();

	// virtual std::vector<const std::shared_ptr<video::texture>> &get_frames() = 0;
};

}
