#include <volt/gpu/vk12/device.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/instance.hpp>
#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/sampler.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/vk12/swapchain.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>
#include <volt/util/util.hpp>
#include <volt/error.hpp>
#include <volt/paths.hpp>

namespace volt::gpu::vk12 {

namespace fs = std::filesystem;
using namespace math;

device::device(std::shared_ptr<gpu::adapter> &&adapter)
		: gpu::device(std::move(adapter)),
		thread_pool(math::min(std::thread::hardware_concurrency(), vk12::max_thread_count)),
		jit(*this) {
	auto &vk12_adapter = *static_cast<vk12::adapter *>(this->_adapter.get());
	auto &vk12_instance = *static_cast<vk12::instance *>(vk12_adapter.instance().get());

	// Queue infos

	std::vector<VkDeviceQueueCreateInfo> queue_infos(vk12_adapter.unique_families.size());
	float priority = 1;

	uint32_t i = 0;
	for (uint32_t family : vk12_adapter.unique_families) {
		queue_infos[i] = {};
		queue_infos[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queue_infos[i].queueFamilyIndex = family;
		queue_infos[i].queueCount = 1;
		queue_infos[i].pQueuePriorities = &priority;
		i++;
	}

	// Device
	VkPhysicalDeviceFeatures device_features{};
	device_features.samplerAnisotropy = VK_TRUE;

	VkPhysicalDeviceRobustness2FeaturesEXT robustness_features{};
	robustness_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
	robustness_features.nullDescriptor = VK_TRUE; // Rquired to bind NULL vertex buffers

	VkDeviceCreateInfo device_info{};
	device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_info.pNext = &robustness_features;
	device_info.queueCreateInfoCount = queue_infos.size();
	device_info.pQueueCreateInfos = queue_infos.data();
	device_info.pEnabledFeatures = &device_features;

#ifdef VOLT_GPU_DEBUG
	auto &layers = vk12::validation_layers();
	device_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
	device_info.ppEnabledLayerNames = layers.data();
#endif

	device_info.ppEnabledExtensionNames = vk12::device_extensions.data();
	device_info.enabledExtensionCount = static_cast<uint32_t>(vk12::device_extensions.size());

	VOLT_VK12_CHECK(vkCreateDevice(vk12_adapter.physical_device, &device_info,
			nullptr, &vk_device), "Failed to create device.")
	vk12::load_glad_device(vk_device);

	// Allocator

	VmaAllocatorCreateInfo allocator_info{};
	allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
	allocator_info.physicalDevice = vk12_adapter.physical_device;
	allocator_info.device = vk_device;
	allocator_info.instance = vk12_instance.vk_instance;
	vmaCreateAllocator(&allocator_info, &allocator);

	// Queues

	vkGetDeviceQueue(vk_device, vk12_adapter.universal_family, 0, &universal_queue);
	vkGetDeviceQueue(vk_device, vk12_adapter.compute_family, 0, &compute_queue);
	vkGetDeviceQueue(vk_device, vk12_adapter.copy_family, 0, &copy_queue);

	// Pipeline cache

	VkPipelineCacheCreateInfo cache_info{};
	cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

	fs::path data_path = paths::data() / "gpu-cache" / "vk12" / (vk12_adapter.pipeline_cache_uuid + ".bin");
	if (fs::exists(data_path)) {
		std::vector<uint8_t> data = util::read_binary_file(data_path);

		cache_info.initialDataSize = data.size();
		cache_info.pInitialData = data.data();
	}

	VOLT_VK12_CHECK(vkCreatePipelineCache(vk_device, &cache_info,
			nullptr, &pipeline_cache), "Failed to create pipeline cache.")
}

device::~device() {
	wait();
	jit.clear();

	// Update pipelines
	auto &vk12_adapter = *static_cast<vk12::adapter *>(this->_adapter.get());

	// Append to previous data - adapter can create multiple devices with common cache
	fs::path data_path = paths::data() / "gpu-cache" / "vk12" / (vk12_adapter.pipeline_cache_uuid + ".bin");
	bool failed = false;
	while (fs::exists(data_path)) { // We use while loop as breakable conditional
		std::vector<uint8_t> data = util::read_binary_file(data_path);

		VkPipelineCacheCreateInfo dst_cache_info{};
		dst_cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

		VkPipelineCacheCreateInfo src_cache_info{};
		src_cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		src_cache_info.initialDataSize = data.size();
		src_cache_info.pInitialData = data.data();

		VkPipelineCache dst_cache, src_cache;
		VkResult result = vkCreatePipelineCache(vk_device, &dst_cache_info,
			nullptr, &dst_cache);
		if (result != VK_SUCCESS) {
			failed = true;
			break;
		}
		result = vkCreatePipelineCache(vk_device, &src_cache_info,
			nullptr, &src_cache);
		if (result != VK_SUCCESS) {
			vkDestroyPipelineCache(vk_device, dst_cache, nullptr);
			failed = true;
			break;
		}

		VkPipelineCache src_caches[] = { src_cache, pipeline_cache };
		vkMergePipelineCaches(vk_device, dst_cache, 2 , src_caches);

		vkDestroyPipelineCache(vk_device, src_cache, nullptr);
		vkDestroyPipelineCache(vk_device, pipeline_cache, nullptr);
		pipeline_cache = dst_cache;
		break;
	}

	// Write merged data
	if (!failed) {
		size_t data_size;
		vkGetPipelineCacheData(vk_device, pipeline_cache, &data_size, nullptr);
		std::vector<uint8_t> data(data_size);
		vkGetPipelineCacheData(vk_device, pipeline_cache, &data_size, data.data());
		util::write_file(data_path, data);
	}

	vkDestroyPipelineCache(vk_device, pipeline_cache, nullptr);
	vmaDestroyAllocator(allocator);
	vkDestroyDevice(vk_device, nullptr);
}

void device::wait() {
	vkDeviceWaitIdle(vk_device);
}

std::shared_ptr<gpu::swapchain> device::create_swapchain(std::shared_ptr<os::window> window) {
	return std::shared_ptr<gpu::swapchain>(new vk12::swapchain(shared_from_this(), std::move(window)));
}

std::shared_ptr<gpu::universal_routine> device::create_universal_routine() {
	return std::shared_ptr<gpu::universal_routine>(new vk12::universal_routine(shared_from_this()));
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

std::shared_ptr<gpu::sampler> device::create_sampler(
		texture_filter filter, bool blur, float anisotropy) {
	return std::shared_ptr<gpu::sampler>(new vk12::sampler(
			shared_from_this(), filter, blur, anisotropy));
}

std::shared_ptr<gpu::shader> device::create_shader(
		const std::vector<uint8_t> &bytecode) {
	return std::shared_ptr<gpu::shader>(new vk12::shader(
			shared_from_this(), bytecode));
}

}
