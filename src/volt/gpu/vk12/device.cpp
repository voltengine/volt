#include <volt/gpu/vk12/device.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/swapchain.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

device::device(std::shared_ptr<gpu::adapter> &&adapter) : gpu::device(std::move(adapter)) {
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

#ifdef VOLT_GPU_DEBUG
	auto &layers = vk12::validation_layers();
	device_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
	device_info.ppEnabledLayerNames = layers.data();
#endif

	device_info.ppEnabledExtensionNames = vk12::device_extensions.data();
	device_info.enabledExtensionCount = vk12::device_extensions.size();

	VOLT_VK12_CHECK(vkCreateDevice(_adapter.physical_device, &device_info,
			nullptr, &vk_device), "Failed to create device.")
	vk12::load_glad_device(vk_device);

	VmaAllocatorCreateInfo allocator_info{};
	allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
	allocator_info.physicalDevice = _adapter.physical_device;
	allocator_info.device = vk_device;
	allocator_info.instance = instance.vk_instance;
	vmaCreateAllocator(&allocator_info, &allocator);

	vkGetDeviceQueue(vk_device, _adapter.graphics_family, 0, &vk_rasterization_queue);
	vkGetDeviceQueue(vk_device, _adapter.compute_family, 0, &vk_compute_queue);
	vkGetDeviceQueue(vk_device, _adapter.transfer_family, 0, &vk_copy_queue);
}

device::~device() {
	wait();
	vmaDestroyAllocator(allocator);
	vkDestroyDevice(vk_device, nullptr);
}

void device::wait() {
	vkDeviceWaitIdle(vk_device);
}

std::shared_ptr<gpu::buffer> device::create_buffer(
		gpu::memory_type memory_type,
		gpu::buffer_features features,
		size_t size) {
	return std::shared_ptr<gpu::buffer>(new vk12::buffer(
			shared_from_this(), memory_type, features, size));
}

std::shared_ptr<gpu::swapchain> device::create_swapchain(std::shared_ptr<os::window> window) {
	return std::shared_ptr<gpu::swapchain>(new vk12::swapchain(shared_from_this(), std::move(window)));
}

std::shared_ptr<gpu::texture> device::create_2d_texture(
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uvec2 size, uint32_t levels) {
	return std::shared_ptr<gpu::texture>(new vk12::texture(
			shared_from_this(), memory_type, features, format, size, levels));
}

}
