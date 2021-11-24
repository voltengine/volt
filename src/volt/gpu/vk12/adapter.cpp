#include <volt/gpu/vk12/adapter.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

adapter::adapter(std::shared_ptr<gpu::instance> &&instance,
		VkPhysicalDevice physical_device, VkSurfaceKHR vk_dummy_surface)
		: gpu::adapter(std::move(instance)), physical_device(physical_device) {
	vk12::load_glad_physical_device(physical_device);

	// Query adapter properties
	vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);
	vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);

	// Queue family details
	uint32_t num_families;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num_families, nullptr);
	families.resize(num_families);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num_families, families.data());

	std::vector<uint32_t> present_score(num_families, 1), graphics_score(num_families, 1),
			compute_score(num_families, 1), transfer_score(num_families, 1);

	for (uint32_t i = 0; i < families.size(); i++) {
		VkBool32 present_support;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, vk_dummy_surface, &present_support);
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
		present_family = std::numeric_limits<uint32_t>::max();
	if (graphics_score[graphics_family] == 0)
		graphics_family = std::numeric_limits<uint32_t>::max();
	if (compute_score[compute_family] == 0)
		compute_family = std::numeric_limits<uint32_t>::max();
	if (transfer_score[transfer_family] == 0)
		transfer_family = std::numeric_limits<uint32_t>::max();

	// Extension support
	uint32_t num_supported_extensions;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_supported_extensions, nullptr);
	supported_extensions.resize(num_supported_extensions);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_supported_extensions, supported_extensions.data());

	// Surface support
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, vk_dummy_surface, &surface_capabilities);

	uint32_t num_formats;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, vk_dummy_surface, &num_formats, nullptr);
	if (num_formats != 0) {
		surface_formats.resize(num_formats);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, vk_dummy_surface, &num_formats, surface_formats.data());
	}

	uint32_t num_modes;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, vk_dummy_surface, &num_modes, nullptr);
	if (num_modes != 0) {
		surface_present_modes.resize(num_modes);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, vk_dummy_surface, &num_modes, surface_present_modes.data());
	}

	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);
}

uint32_t adapter::vendor_id() {
	return physical_device_properties.vendorID;
}

uint32_t adapter::device_id() {
	return physical_device_properties.deviceID;
}

std::string adapter::name() {
	return physical_device_properties.deviceName;
}

uint64_t adapter::dedicated_video_memory() {
	static VkDeviceSize memory = [&]() {
		VkDeviceSize memory = 0;
		for (uint32_t i = 0; i < physical_device_memory_properties.memoryHeapCount; i++) {
			VkMemoryHeap heap = physical_device_memory_properties.memoryHeaps[i];
			if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				memory += heap.size;
		}
		return memory;
	}();
	
	return memory;
}

std::shared_ptr<gpu::device> adapter::create_device() {
	return std::make_shared<vk12::device>(shared_from_this());
}

}
