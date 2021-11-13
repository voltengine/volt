#include <volt/pch.hpp>
#include <volt/video/vk12/device.hpp>

#include <volt/math/math.hpp>
#include <volt/video/vk12/shader.hpp>
#include <volt/video/vk12/swapchain.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12::_internal {

device::device(_internal::adapter &adapter) : adapter(adapter) {
	// Group queue types with common families
	// Queue types are 0, 1, 2 and 3 in the following order
	std::array<uint32_t, 4> type_to_family{
		adapter.present_family,  // 0
		adapter.graphics_family, // 1
		adapter.compute_family,  // 2
		adapter.transfer_family  // 3
	};
	
	// Queue family index + Set of queue types
	std::map<uint32_t, std::set<uint32_t>> family_to_types;
	for (uint32_t i = 0; i < type_to_family.size(); i++)
		family_to_types[type_to_family[i]].emplace(i);

	std::vector<VkDeviceQueueCreateInfo> queue_infos(family_to_types.size());
	std::array<float, 4> queue_priorities;
	queue_priorities.fill(1);

	uint32_t i = 0;
	for (auto &item : family_to_types) {
		queue_infos[i] = {};
		queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_infos[i].queueFamilyIndex = item.first;
		queue_infos[i].queueCount = item.second.size();
		queue_infos[i].pQueuePriorities = queue_priorities.data();
		i++;
	}

	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = queue_infos.size();
	device_info.pQueueCreateInfos = queue_infos.data();

	VkPhysicalDeviceFeatures device_features{};
	device_info.pEnabledFeatures = &device_features;

#ifndef NDEBUG
	auto &layers = _internal::validation_layers();
	device_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
	device_info.ppEnabledLayerNames = layers.data();
#endif

	device_info.ppEnabledExtensionNames = _internal::device_extensions.data();
	device_info.enabledExtensionCount = _internal::device_extensions.size();

	VOLT_ASSERT(vkCreateDevice(adapter.vk_physical_device,
			&device_info, nullptr, &vk_device)
			== VK_SUCCESS, "Failed to create device.")
	VOLT_ASSERT(gladLoaderLoadVulkan(adapter.vk_instance, adapter.vk_physical_device, vk_device),
			"Failed to load Vulkan device symbols.")
}

device::~device() {
	vkDestroyDevice(vk_device, nullptr);
}

std::shared_ptr<video::swapchain> device::create_swapchain(const std::shared_ptr<video::window> &window) {
	return std::make_shared<_internal::swapchain>(*this, *std::static_pointer_cast<_internal::window>(window));
}

std::shared_ptr<video::shader> device::create_shader(
		const std::vector<uint8_t> &bytecode) {
	return std::make_shared<_internal::shader>(vk_device, bytecode);
}

std::shared_ptr<video::pipeline> device::create_pipeline() {
	return nullptr; // TODO: Implement
}

}
