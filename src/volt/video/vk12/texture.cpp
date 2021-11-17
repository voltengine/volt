#include <volt/video/vk12/texture.hpp>

#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/texture_view.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12 {

using namespace math;

texture::texture(std::shared_ptr<video::device> &&device,
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		uint32_t size, uint32_t levels, uint32_t layers,
		video::texture_format format)
		: video::texture(std::move(device)),
		_device(*static_cast<vk12::device *>(this->device.get())) {
	VkImageCreateInfo image_info{};
	image_info.imageType = VK_IMAGE_TYPE_1D;
	image_info.extent.width = size;
	image_info.extent.height = 1;
	image_info.extent.depth = 1;
	create(std::move(image_info), resource_type,
			sync_queues, features, levels, format);
}

texture::texture(std::shared_ptr<video::device> &&device,
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		math::uvec2 size, uint32_t levels, uint32_t layers,
		video::texture_format format)
		: video::texture(std::move(device)),
		_device(*static_cast<vk12::device *>(this->device.get())) {
	VkImageCreateInfo image_info{};
	image_info.imageType = VK_IMAGE_TYPE_2D;
	image_info.extent.width = size.x;
	image_info.extent.height = size.y;
	image_info.extent.depth = 1;
	create(std::move(image_info), resource_type,
			sync_queues, features, levels, format);
}

texture::texture(std::shared_ptr<video::device> &&device,
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		math::uvec3 size, uint32_t levels, uint32_t layers,
		video::texture_format format)
		: video::texture(std::move(device)),
		_device(*static_cast<vk12::device *>(this->device.get())) {
	VkImageCreateInfo image_info{};
	image_info.imageType = VK_IMAGE_TYPE_3D;
	image_info.extent.width = size.x;
	image_info.extent.height = size.y;
	image_info.extent.depth = size.z;
	create(std::move(image_info), resource_type,
			sync_queues, features, levels, format);
}

texture::texture(std::shared_ptr<video::device> &&device, VkImage image, VkFormat format) :
		video::texture(std::move(device)),
		_device(*static_cast<vk12::device *>(this->device.get())),
		image(image), format(format) {}

texture::~texture() {
	if (allocation)
		vmaDestroyImage(_device.allocator, image, allocation);
}

std::shared_ptr<video::texture_view> texture::create_view(
		video::texture_view_type type,
		video::texture_view_aspects aspects) {
	return std::shared_ptr<video::texture_view>(static_cast<video::texture_view *>(
			new vk12::texture_view(shared_from_this(), type, aspects)));
}

void texture::map(void **ptr) {
	VOLT_VK12_DEBUG_CHECK(vmaMapMemory(_device.allocator, allocation, ptr),
			"Failed to unmap memory.");
}

void texture::unmap() {
	vmaUnmapMemory(_device.allocator, allocation);
}

void texture::read(size_t offset, size_t size) {
	VOLT_VK12_DEBUG_CHECK(vmaInvalidateAllocation(_device.allocator, allocation, offset, size),
			"Failed to invalidate allocation.");
}

void texture::write(size_t offset, size_t size) {
	VOLT_VK12_DEBUG_CHECK(vmaFlushAllocation(_device.allocator, allocation, offset, size),
			"Failed to flush allocation.");
}

void texture::create(VkImageCreateInfo &&image_info,
		video::resource_type resource_type,
		video::sync_queues sync_queues,
		video::texture_features features,
		uint32_t levels, video::texture_format format) {
	auto &adapter = *static_cast<vk12::adapter *>(_device.get_adapter().get());

	VkBufferUsageFlags buffer_usage = (features & 0b11) | ((features & 0b111100) << 2);
	// Buffer will never be used in presentation queue
	std::set<uint32_t> unique_families;
	if (sync_queues & video::sync_queue::graphics)
		unique_families.emplace(adapter.graphics_family);
	if (sync_queues & video::sync_queue::compute)
		unique_families.emplace(adapter.compute_family);
	if (sync_queues & video::sync_queue::copy)
		unique_families.emplace(adapter.transfer_family);
	std::vector<uint32_t> families(unique_families.begin(), unique_families.end());

	image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_info.format = this->format = vk12::texture_formats[format];
	image_info.mipLevels = levels;
	image_info.arrayLayers = 1;
	image_info.samples = VK_SAMPLE_COUNT_1_BIT; // Use for MSAA
	image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_info.usage = features & 0b111111;
	image_info.sharingMode = (families.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
	image_info.queueFamilyIndexCount = families.size();
	image_info.pQueueFamilyIndices = families.data();
	image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	
	VmaAllocationCreateInfo allocation_info = {};
	allocation_info.usage = vk12::vma_memory_usages[resource_type];

	vmaCreateImage(_device.allocator, &image_info, &allocation_info, &image, &allocation, nullptr);
}

}

