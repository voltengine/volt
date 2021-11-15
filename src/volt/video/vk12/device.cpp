#include <volt/pch.hpp>
#include <volt/video/vk12/device.hpp>

#include <volt/math/math.hpp>
#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/buffer.hpp>
#include <volt/video/vk12/shader.hpp>
#include <volt/video/vk12/surface.hpp>
#include <volt/video/vk12/texture.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12 {

device::device(std::shared_ptr<video::adapter> &&adapter) : video::device(std::move(adapter)) {
	auto &_adapter = *static_cast<vk12::adapter *>(this->adapter.get());
	auto &instance = *static_cast<vk12::instance *>(this->adapter->get_instance().get());

	// Group queue types with common families
	// Queue types are 0, 1, 2 and 3 in the following order
	std::array<uint32_t, 4> type_to_family{
		_adapter.present_family,  // 0
		_adapter.graphics_family, // 1
		_adapter.compute_family,  // 2
		_adapter.transfer_family  // 3
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
	auto &layers = vk12::validation_layers();
	device_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
	device_info.ppEnabledLayerNames = layers.data();
#endif

	device_info.ppEnabledExtensionNames = vk12::device_extensions.data();
	device_info.enabledExtensionCount = vk12::device_extensions.size();

	VOLT_ASSERT(vkCreateDevice(_adapter.physical_device, &device_info, nullptr, &vk_device)
			== VK_SUCCESS, "Failed to create device.")
	VOLT_ASSERT(gladLoaderLoadVulkan(instance.vk_instance, _adapter.physical_device, vk_device),
			"Failed to load Vulkan device symbols.")

	VmaAllocatorCreateInfo allocator_info{};
	allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
	allocator_info.physicalDevice = _adapter.physical_device;
	allocator_info.device = vk_device;
	allocator_info.instance = instance.vk_instance;
	vmaCreateAllocator(&allocator_info, &allocator);

	vkGetDeviceQueue(vk_device, _adapter.graphics_family, 0, &graphics_queue.vk_queue);
	vkGetDeviceQueue(vk_device, _adapter.compute_family, 0, &compute_queue.vk_queue);
	vkGetDeviceQueue(vk_device, _adapter.transfer_family, 0, &transfer_queue.vk_queue);
}

device::~device() {
	vmaDestroyAllocator(allocator);
	vkDestroyDevice(vk_device, nullptr);
}

void device::wait() {
	vkDeviceWaitIdle(vk_device);
}

video::queue &device::get_graphics_queue() {
	return graphics_queue;
}

video::queue &device::get_compute_queue() {
	return compute_queue;
}

video::queue &device::get_copy_queue() {
	return transfer_queue;
}

std::shared_ptr<video::buffer> device::create_buffer(
		video::resource::type resource_type,
		video::queue::types sync_queues,
		video::buffer::features features,
		size_t size) {
	return std::shared_ptr<video::buffer>(new vk12::buffer(
			shared_from_this(), resource_type, sync_queues, features, size));
}

std::shared_ptr<video::surface> device::create_surface(std::shared_ptr<os::window> window) {
	return std::shared_ptr<video::surface>(new vk12::surface(shared_from_this(), std::move(window)));
}

std::shared_ptr<video::texture> device::create_texture(
		video::resource::type resource_type,
		video::queue::types sync_queues,
		video::texture::features features,
		size_t size, uint32_t levels, uint32_t layers,
		video::texture::format format) {
	return std::shared_ptr<video::texture>(new vk12::texture(
			shared_from_this(), resource_type, sync_queues, features, size, levels, layers, format));
}

std::shared_ptr<video::texture> device::create_texture(
		video::resource::type resource_type,
		video::queue::types sync_queues,
		video::texture::features features,
		math::uvec2 size, uint32_t levels, uint32_t layers,
		video::texture::format format) {
	return std::shared_ptr<video::texture>(new vk12::texture(
			shared_from_this(), resource_type, sync_queues, features, size, levels, layers, format));
}

std::shared_ptr<video::texture> device::create_texture(
		video::resource::type resource_type,
		video::queue::types sync_queues,
		video::texture::features features,
		math::uvec3 size, uint32_t levels, uint32_t layers,
		video::texture::format format) {
	return std::shared_ptr<video::texture>(new vk12::texture(
			shared_from_this(), resource_type, sync_queues, features, size, levels, layers, format));
}

std::shared_ptr<video::shader> device::create_shader(
		const std::vector<uint8_t> &bytecode) {
	return std::shared_ptr<video::shader>(new vk12::shader(vk_device, bytecode));
}

// std::shared_ptr<video::pipeline> device::create_pipeline() {
// 	return nullptr; // TODO: Implement
// }

}
