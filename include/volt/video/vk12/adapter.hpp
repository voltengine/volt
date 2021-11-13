#pragma once

#include "../../macros.hpp"

#include <glad/vulkan.h>

#include "../adapter.hpp"

namespace volt::video::vk12::_internal {

class adapter : public video::adapter {
public:
	VkInstance vk_instance;
	VkPhysicalDevice vk_physical_device;
	VkPhysicalDeviceProperties vk_physical_device_properties;
	VkPhysicalDeviceMemoryProperties vk_physical_device_memory_properties;
	std::vector<VkQueueFamilyProperties> families;
	uint32_t present_family, graphics_family, compute_family, transfer_family;
	std::vector<VkExtensionProperties> supported_extensions;
	VkSurfaceCapabilitiesKHR surface_capabilities;
	std::vector<VkSurfaceFormatKHR> surface_formats;
	std::vector<VkPresentModeKHR> surface_present_modes;

	VOLT_API adapter(VkInstance vk_instance, VkPhysicalDevice vk_physical_device, VkSurfaceKHR vk_dummy_surface);

	VOLT_API std::shared_ptr<video::device> create_device() override;

	VOLT_API uint32_t vendor_id() override;

	VOLT_API uint32_t device_id() override;

	VOLT_API std::string name() override;

	VOLT_API uint64_t dedicated_video_memory() override;
};

}
