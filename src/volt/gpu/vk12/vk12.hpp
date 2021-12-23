#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/gpu.hpp>

namespace volt::gpu::vk12 {

extern std::unordered_map<VkResult, std::string> result_messages;

extern std::vector<const char *> device_extensions;

extern std::unordered_map<gpu::memory_type, VmaMemoryUsage> vma_memory_usages;

extern std::unordered_map<gpu::texture_format, VkFormat> texture_formats;

constexpr uint32_t concurrent_frames = 2;

extern std::filesystem::path cache_path;

void load_glad();

void unload_glad();

void load_glad_instance(VkInstance instance);

void load_glad_physical_device(VkPhysicalDevice physical_device);

void load_glad_device(VkDevice device);

const std::vector<const char *> &validation_layers();

}

#define VOLT_VK12_CHECK(expression, message)\
{\
	::VkResult _volt_vk12_check_result = expression;\
	VOLT_ASSERT(_volt_vk12_check_result == VK_SUCCESS, message + ('\n' + ::volt::gpu::vk12::result_messages[_volt_vk12_check_result]))\
}

#ifdef VOLT_GPU_DEBUG
	#define VOLT_VK12_DEBUG_CHECK(expression, message) VOLT_VK12_CHECK(expression, message)
#else
	#define VOLT_VK12_DEBUG_CHECK(expression, message) expression;
#endif
