#pragma once

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

	adapter(std::shared_ptr<video::instance> &&instance, VkPhysicalDevice physical_device, VkSurfaceKHR vk_dummy_surface);

	std::vector<uint32_t> unique_families(video::sync_queues sync_queues);

	uint32_t vendor_id() override;

	uint32_t device_id() override;

	std::string name() override;

	uint64_t dedicated_video_memory() override;

	std::shared_ptr<video::device> create_device() override;
};

}
