#pragma once

#include "../../macros.hpp"

#include <glad/vulkan.h>

#include "../swapchain.hpp"
#include "device.hpp"
#include "window.hpp"

namespace volt::video::vk12::_internal {

class swapchain : public video::swapchain {
public:
	_internal::device &device;
	VkSwapchainKHR vk_swapchain;

	VOLT_API swapchain(_internal::device &device, _internal::window &window);

	VOLT_API ~swapchain();
};

}