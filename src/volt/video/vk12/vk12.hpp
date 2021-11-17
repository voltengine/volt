#pragma once

#include <volt/macros.hpp>

#include <vector>

#include <volt/video/video.hpp>

namespace volt::video::vk12 {

extern std::map<VkResult, std::string> result_messages;

extern std::vector<const char *> device_extensions;

extern std::map<video::resource_type, VmaMemoryUsage> vma_memory_usages;

extern std::map<video::texture_format, VkFormat> texture_formats;

const std::vector<const char *> &validation_layers();

}

#define VOLT_VK12_CHECK(expression, message)\
{\
	::VkResult result = expression;\
	VOLT_ASSERT(result == VK_SUCCESS, message + ('\n' + ::volt::video::vk12::result_messages[result]))\
}

#ifdef VOLT_VIDEO_DEBUG
	#define VOLT_VK12_DEBUG_CHECK(expression, message) VOLT_VK12_CHECK(expression, message)
#else
	#define VOLT_VK12_DEBUG_CHECK(expression, message) expression;
#endif


#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/device.hpp>
#include <volt/video/vk12/instance.hpp>
#include <volt/video/vk12/surface.hpp>
