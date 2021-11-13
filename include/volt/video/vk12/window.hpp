#pragma once

#include "../../macros.hpp"

#include <glad/vulkan.h>

#include "../window.hpp"

namespace volt::video::vk12::_internal {

class window : public video::window {
public:
	VkInstance vk_instance;
	VkSurfaceKHR vk_surface;

	VOLT_API window(std::string &&title, math::uvec2 size, VkInstance vk_instance);

	VOLT_API ~window();
};

}
