#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/adapter.hpp>
#include <volt/gpu/instance.hpp>

namespace volt::gpu::vk12 {

class adapter : public gpu::adapter {
public:
	VkPhysicalDevice physical_device;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceMemoryProperties memory_properties;
	std::vector<VkQueueFamilyProperties> families;
	uint32_t universal_family, compute_family, copy_family;
	std::vector<uint32_t> unique_families;
	std::vector<VkExtensionProperties> supported_extensions;
	std::vector<VkSurfaceFormatKHR> surface_formats;
	std::vector<VkPresentModeKHR> surface_present_modes;
	std::string pipeline_cache_uuid;

	adapter(std::shared_ptr<gpu::instance> &&instance, VkPhysicalDevice physical_device, VkSurfaceKHR vk_dummy_surface);

	std::string vendor() override;

	std::string name() override;

	uint64_t total_memory() override;

	uint64_t free_memory() override;

	std::shared_ptr<gpu::device> create_device() override;
};

}
