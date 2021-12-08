#include <volt/gpu/vk12/device.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/swapchain.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

device::device(std::shared_ptr<gpu::adapter> &&adapter) : gpu::device(std::move(adapter)) {
	auto &_adapter = *static_cast<vk12::adapter *>(this->_adapter.get());
	auto &instance = *static_cast<vk12::instance *>(_adapter.instance().get());

	std::set<uint32_t> families{
		_adapter.present_family,  // 0
		_adapter.graphics_family, // 1
		_adapter.compute_family,  // 2
		_adapter.copy_family      // 3
	};

	std::vector<VkDeviceQueueCreateInfo> queue_infos(families.size());
	std::array<float, 4> queue_priorities;
	queue_priorities.fill(1);

	uint32_t i = 0;
	for (uint32_t family : families) {
		queue_infos[i] = {};
		queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_infos[i].queueFamilyIndex = family;
		queue_infos[i].queueCount = 1;
		queue_infos[i].pQueuePriorities = queue_priorities.data();
		i++;
	}

	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.queueCreateInfoCount = queue_infos.size();
	device_info.pQueueCreateInfos = queue_infos.data();

	VkPhysicalDeviceFeatures device_features{};
	device_features.samplerAnisotropy = VK_TRUE;
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

	vkGetDeviceQueue(vk_device, _adapter.present_family, 0, &present_queue);
	vkGetDeviceQueue(vk_device, _adapter.graphics_family, 0, &graphics_queue);
	vkGetDeviceQueue(vk_device, _adapter.compute_family, 0, &compute_queue);
	vkGetDeviceQueue(vk_device, _adapter.copy_family, 0, &copy_queue);
}

device::~device() {
	wait();
	vmaDestroyAllocator(allocator);
	vkDestroyDevice(vk_device, nullptr);
}

void device::wait() {
	vkDeviceWaitIdle(vk_device);
}

std::shared_ptr<gpu::swapchain> device::create_swapchain(std::shared_ptr<os::window> window) {
	return std::shared_ptr<gpu::swapchain>(new vk12::swapchain(shared_from_this(), std::move(window)));
}

std::shared_ptr<gpu::graphics_routine> device::create_graphics_routine() {
	return std::shared_ptr<gpu::graphics_routine>(new vk12::graphics_routine(shared_from_this()));
}

std::shared_ptr<gpu::compute_routine> device::create_compute_routine() {
	return std::shared_ptr<gpu::compute_routine>(new vk12::compute_routine(shared_from_this()));
}

std::shared_ptr<gpu::copy_routine> device::create_copy_routine() {
	return std::shared_ptr<gpu::copy_routine>(new vk12::copy_routine(shared_from_this()));
}

std::shared_ptr<gpu::buffer> device::create_buffer(
		gpu::memory_type memory_type,
		gpu::buffer_features features,
		size_t size) {
	return std::shared_ptr<gpu::buffer>(new vk12::buffer(
			shared_from_this(), memory_type, features, size));
}

std::shared_ptr<gpu::texture> device::create_1d_texture(
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uint32_t levels, uint32_t size, uint32_t layers) {
	vk12::texture::type type = layers == 1 ? vk12::texture::type::tex1d : vk12::texture::type::tex1d_array;
	return std::shared_ptr<gpu::texture>(new vk12::texture(
			shared_from_this(), memory_type, features, format, levels, uvec3(size, 1, layers), type));
}

std::shared_ptr<gpu::texture> device::create_2d_texture(
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uint32_t levels, math::uvec2 size, uint32_t layers) {
	vk12::texture::type type = layers == 1 ? vk12::texture::type::tex2d : vk12::texture::type::tex2d_array;
	return std::shared_ptr<gpu::texture>(new vk12::texture(
			shared_from_this(), memory_type, features, format, levels, uvec3(size, layers), type));
}

std::shared_ptr<gpu::texture> device::create_3d_texture(
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uint32_t levels, math::uvec3 size) {
	return std::shared_ptr<gpu::texture>(new vk12::texture(
			shared_from_this(), memory_type, features, format, levels, size, vk12::texture::type::tex3d));
}

std::shared_ptr<gpu::texture> device::create_cube_texture(
		gpu::memory_type memory_type,
		gpu::texture_features features,
		gpu::texture_format format,
		uint32_t levels, math::uvec2 size, uint32_t layers) {
	vk12::texture::type type = layers == 1 ? vk12::texture::type::cube : vk12::texture::type::cube_array;
	return std::shared_ptr<gpu::texture>(new vk12::texture(
			shared_from_this(), memory_type, features, format, levels, uvec3(size, layers * 6), type));
}

}
