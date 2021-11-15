#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/instance.hpp>

namespace volt::video::vk12 {

class instance : public video::instance {
public:
	VkInstance vk_instance;
#ifndef NDEBUG
	VkDebugUtilsMessengerEXT vk_messenger;
#endif

	VOLT_API instance();

	VOLT_API ~instance();

	VOLT_API std::vector<std::shared_ptr<video::adapter>> list_adapters() override;
};

}
