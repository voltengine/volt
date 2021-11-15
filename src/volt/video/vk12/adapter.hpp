#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/adapter.hpp>
#include <volt/video/instance.hpp>
#include <volt/video/queue.hpp>

namespace volt::video::vk12 {

class adapter : public video::adapter {
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

	VOLT_API adapter(std::shared_ptr<video::instance> &&instance, VkPhysicalDevice physical_device, VkSurfaceKHR vk_dummy_surface);

	VOLT_API std::vector<uint32_t> unique_families(video::queue::types sync_queues);

	VOLT_API uint32_t vendor_id() override;

	VOLT_API uint32_t device_id() override;

	VOLT_API std::string name() override;

	VOLT_API uint64_t dedicated_video_memory() override;

	VOLT_API std::shared_ptr<video::device> create_device() override;
};

}
