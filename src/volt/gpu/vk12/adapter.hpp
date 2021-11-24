#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/adapter.hpp>
#include <volt/gpu/instance.hpp>

namespace volt::gpu::vk12 {

class adapter : public gpu::adapter {
public:
	VkPhysicalDevice physical_device;
	VkPhysicalDeviceProperties physical_device_properties;
	VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
	std::vector<VkQueueFamilyProperties> families;
	uint32_t present_family, graphics_family, compute_family, transfer_family;
	std::vector<VkExtensionProperties> supported_extensions;
	VkSurfaceCapabilitiesKHR surface_capabilities;
	std::vector<VkSurfaceFormatKHR> surface_formats;
	std::vector<VkPresentModeKHR> surface_present_modes;
	VkPhysicalDeviceMemoryProperties memory_properties;

	adapter(std::shared_ptr<gpu::instance> &&instance, VkPhysicalDevice physical_device, VkSurfaceKHR vk_dummy_surface);

	uint32_t vendor_id() override;

	uint32_t device_id() override;

	std::string name() override;

	uint64_t dedicated_video_memory() override;

	std::shared_ptr<gpu::device> create_device() override;
};

}
