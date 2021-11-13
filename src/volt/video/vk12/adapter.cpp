#include <volt/pch.hpp>
#include <volt/video/vk12/adapter.hpp>

#include <volt/video/vk12/device.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12::_internal {

adapter::adapter(VkInstance vk_instance, VkPhysicalDevice
		vk_physical_device, VkSurfaceKHR vk_dummy_surface)
		: vk_instance(vk_instance), vk_physical_device(vk_physical_device) {
	VOLT_ASSERT(gladLoaderLoadVulkan(vk_instance, vk_physical_device, nullptr),
			"Failed to load Vulkan physical device symbols.")

	// Query adapter properties
	vkGetPhysicalDeviceProperties(vk_physical_device, &vk_physical_device_properties);
	vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &vk_physical_device_memory_properties);

	// Queue family details
	uint32_t num_families;
	vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &num_families, nullptr);
	families.resize(num_families);
	vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &num_families, families.data());

	std::vector<uint32_t> present_score(num_families, 1), graphics_score(num_families, 1),
			compute_score(num_families, 1), transfer_score(num_families, 1);

	for (uint32_t i = 0; i < families.size(); i++) {
		VkBool32 present_support;
		vkGetPhysicalDeviceSurfaceSupportKHR(vk_physical_device, i, vk_dummy_surface, &present_support);
		auto family = families[i];
		bool transfer_support = family.queueFlags & VK_QUEUE_TRANSFER_BIT
				|| family.queueFlags & VK_QUEUE_COMPUTE_BIT
				|| family.queueFlags & VK_QUEUE_GRAPHICS_BIT;

		// We must remember that VK_QUEUE_TRANSFER_BIT is optional and implicitly
		// present when VK_QUEUE_GRAPHICS_BIT or VK_QUEUE_COMPUTE_BIT are there
		present_score[i] += !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT);
		present_score[i] += !(family.queueFlags & VK_QUEUE_COMPUTE_BIT);
		present_score[i] += !transfer_support;
		
		graphics_score[i] += (present_support == 0);
		graphics_score[i] += !(family.queueFlags & VK_QUEUE_COMPUTE_BIT);
		graphics_score[i] += !transfer_support;

		compute_score[i] += (present_support == 0);
		compute_score[i] += !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT);
		compute_score[i] += !transfer_support;

		transfer_score[i] += (present_support == 0);
		transfer_score[i] += !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT);
		transfer_score[i] += !(family.queueFlags & VK_QUEUE_COMPUTE_BIT);

		present_score[i] *= (present_support != 0);
		graphics_score[i] *= family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
		compute_score[i] *= family.queueFlags & VK_QUEUE_COMPUTE_BIT;
		transfer_score[i] *= transfer_support;
	}

	present_family = std::distance(present_score.begin(), std::max_element(present_score.begin(), present_score.end()));
	graphics_family = std::distance(graphics_score.begin(), std::max_element(graphics_score.begin(), graphics_score.end()));
	compute_family = std::distance(compute_score.begin(), std::max_element(compute_score.begin(), compute_score.end()));
	transfer_family = std::distance(transfer_score.begin(), std::max_element(transfer_score.begin(), transfer_score.end()));

	if (present_score[present_family] == 0)
		present_family = -1;
	if (graphics_score[graphics_family] == 0)
		graphics_family = -1;
	if (compute_score[compute_family] == 0)
		compute_family = -1;
	if (transfer_score[transfer_family] == 0)
		transfer_family = -1;

	// Extension support
	uint32_t num_supported_extensions;
	vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &num_supported_extensions, nullptr);
	supported_extensions.resize(num_supported_extensions);
	vkEnumerateDeviceExtensionProperties(vk_physical_device, nullptr, &num_supported_extensions, supported_extensions.data());

	// Surface support
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vk_physical_device, vk_dummy_surface, &surface_capabilities);

	uint32_t num_formats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_dummy_surface, &num_formats, nullptr);
	if (num_formats != 0) {
		surface_formats.resize(num_formats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(vk_physical_device, vk_dummy_surface, &num_formats, surface_formats.data());
	}

	uint32_t num_modes;
	vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_dummy_surface, &num_modes, nullptr);
	if (num_modes != 0) {
		surface_present_modes.resize(num_modes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(vk_physical_device, vk_dummy_surface, &num_modes, surface_present_modes.data());
	}
}

std::shared_ptr<video::device> adapter::create_device() {
	return std::make_shared<_internal::device>(*this);
}

uint32_t adapter::vendor_id() {
	return vk_physical_device_properties.vendorID;
}

uint32_t adapter::device_id() {
	return vk_physical_device_properties.deviceID;
}

std::string adapter::name() {
	return vk_physical_device_properties.deviceName;
}

uint64_t adapter::dedicated_video_memory() {
	static VkDeviceSize memory = [&]() {
		VkDeviceSize memory = 0;
		for (uint32_t i = 0; i < vk_physical_device_memory_properties.memoryHeapCount; i++) {
			VkMemoryHeap heap = vk_physical_device_memory_properties.memoryHeaps[i];
			if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				memory += heap.size;
		}
		return memory;
	}();
	
	return memory;
}

}
