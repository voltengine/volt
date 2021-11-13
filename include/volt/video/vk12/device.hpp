#pragma once

#include "../../macros.hpp"

#include <glad/vulkan.h>

#include "../device.hpp"
#include "adapter.hpp"

namespace volt::video::vk12::_internal {

class device : public video::device {
public:
	_internal::adapter &adapter;
	VkDevice vk_device;

	VOLT_API device(_internal::adapter &adapter);
	
	VOLT_API ~device();

	VOLT_API std::shared_ptr<video::swapchain> create_swapchain(const std::shared_ptr<video::window> &window) override;

	VOLT_API std::shared_ptr<video::shader> create_shader(const std::vector<uint8_t> &bytecode) override;

	VOLT_API std::shared_ptr<video::pipeline> create_pipeline() override;
};

}
