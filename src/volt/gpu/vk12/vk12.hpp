#pragma once

#include <volt/macros.hpp>

#include <vector>

#include <volt/gpu/gpu.hpp>

namespace volt::gpu::vk12 {

extern std::map<VkResult, std::string> result_messages;

extern std::vector<const char *> device_extensions;

extern std::map<gpu::resource_type, VmaMemoryUsage> vma_memory_usages;

extern std::map<gpu::texture_format, VkFormat> texture_formats;

const std::vector<const char *> &validation_layers();

}

#define VOLT_VK12_CHECK(expression, message)\
{\
	::VkResult result = expression;\
	VOLT_ASSERT(result == VK_SUCCESS, message + ('\n' + ::volt::gpu::vk12::result_messages[result]))\
}

#ifdef VOLT_GPU_DEBUG
	#define VOLT_VK12_DEBUG_CHECK(expression, message) VOLT_VK12_CHECK(expression, message)
#else
	#define VOLT_VK12_DEBUG_CHECK(expression, message) expression;
#endif


#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/surface.hpp>
