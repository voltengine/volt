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
	vkGetPhysicalDeviceProperties(physical_device, &properties);
	vkGetPhysicalDeviceMemoryProperties(physical_device, &memory_properties);

	// Queue family details
	uint32_t num_families;
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num_families, nullptr);
	families.resize(num_families);
	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &num_families, families.data());

	std::vector<uint32_t> universal_score(num_families, 1),
			compute_score(num_families, 1), copy_score(num_families, 1);

	for (uint32_t i = 0; i < families.size(); i++) {
		VkBool32 present_support;
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, vk_dummy_surface, &present_support);
		auto family = families[i];
		bool copy_support = family.queueFlags & VK_QUEUE_TRANSFER_BIT
				|| family.queueFlags & VK_QUEUE_COMPUTE_BIT
				|| family.queueFlags & VK_QUEUE_GRAPHICS_BIT;

		compute_score[i] += (present_support == 0);
		compute_score[i] += !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT);

		copy_score[i] += (present_support == 0);
		copy_score[i] += !(family.queueFlags & VK_QUEUE_GRAPHICS_BIT);
		copy_score[i] += !(family.queueFlags & VK_QUEUE_COMPUTE_BIT);

		universal_score[i] *= (present_support != 0) && (family.queueFlags & VK_QUEUE_GRAPHICS_BIT) && (family.queueFlags & VK_QUEUE_COMPUTE_BIT); // copy_support is guaranteed
		compute_score[i] *= static_cast<bool>(family.queueFlags & VK_QUEUE_COMPUTE_BIT); // copy_support is guaranteed
		copy_score[i] *= copy_support;
	}

	universal_family = std::distance(universal_score.begin(), std::max_element(universal_score.begin(), universal_score.end()));
	compute_family = std::distance(compute_score.begin(), std::max_element(compute_score.begin(), compute_score.end()));
	copy_family = std::distance(copy_score.begin(), std::max_element(copy_score.begin(), copy_score.end()));

	if (universal_score[universal_family] == 0)
		universal_family = std::numeric_limits<uint32_t>::max();
	if (compute_score[compute_family] == 0)
		compute_family = std::numeric_limits<uint32_t>::max();
	if (copy_score[copy_family] == 0)
		copy_family = std::numeric_limits<uint32_t>::max();

	std::set<uint32_t> families{
		universal_family,
		compute_family,
		copy_family
	};
	unique_families.assign(families.begin(), families.end());

	// Extension support
	uint32_t num_supported_extensions;
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_supported_extensions, nullptr);
	supported_extensions.resize(num_supported_extensions);
	vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &num_supported_extensions, supported_extensions.data());

	// Surface support
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

	std::stringstream ss;
	ss << std::hex;
	for (size_t i = 0; i < VK_UUID_SIZE; i++)
		ss << static_cast<uint32_t>(properties.pipelineCacheUUID[i]);
	
	pipeline_cache_uuid = ss.str();
}

std::string adapter::vendor() {
	return vk12::vendor_names[properties.vendorID];
}

std::string adapter::name() {
	return properties.deviceName;
}

uint64_t adapter::total_memory() {
	VkDeviceSize memory = 0;

	for (uint32_t i = 0; i < memory_properties.memoryHeapCount; i++) {
		VkMemoryHeap heap = memory_properties.memoryHeaps[i];
		if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
			memory += heap.size;
	}

	return memory;
}

uint64_t adapter::free_memory() {
	VkPhysicalDeviceMemoryBudgetPropertiesEXT budget_properties{};
	budget_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_BUDGET_PROPERTIES_EXT;

	VkPhysicalDeviceMemoryProperties2 memory_properties2;
	memory_properties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
	memory_properties2.pNext = &budget_properties;

	vkGetPhysicalDeviceMemoryProperties2(physical_device, &memory_properties2);

	VkDeviceSize memory = 0;
	
	for (uint32_t i = 0; i < memory_properties.memoryHeapCount; i++)
		memory += budget_properties.heapBudget[i];

	return memory;
}

std::shared_ptr<gpu::device> adapter::create_device() {
	return std::make_shared<vk12::device>(shared_from_this());
}

}
