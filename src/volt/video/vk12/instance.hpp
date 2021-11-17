#pragma once

#include <volt/pch.hpp>

#include <volt/video/instance.hpp>

namespace volt::video::vk12 {

class instance : public video::instance {
public:
	VkInstance vk_instance;
#ifdef VOLT_VIDEO_DEBUG
	VkDebugUtilsMessengerEXT vk_messenger;
#endif

	instance();

	~instance();

	std::vector<std::shared_ptr<video::adapter>> enumerate_adapters() override;
};

}
