#pragma once

#include "../../macros.hpp"

#include <glad/vulkan.h>

#include "../instance.hpp"

namespace volt::video::vk12 {

class instance : public video::instance {
public:
	VOLT_API instance();

	VOLT_API ~instance();

	VOLT_API std::vector<std::shared_ptr<video::adapter>> list_adapters() override;

	VOLT_API std::shared_ptr<video::window> create_window(std::string title, math::uvec2 size) override;
	
private:
	VkInstance vk_instance;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT vk_messenger;
#endif
};

}
