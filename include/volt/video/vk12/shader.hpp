#pragma once

#include "../../macros.hpp"

#include <glad/vulkan.h>

#include "../shader.hpp"

namespace volt::video::vk12::_internal {

class shader : public video::shader {
public:
	VkDevice vk_device;
	VkShaderModule vk_module;

	VOLT_API shader(VkDevice vk_device, const std::vector<uint8_t> &bytecode);

	VOLT_API ~shader();
};

}