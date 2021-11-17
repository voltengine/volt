#include <volt/gpu/vk12/buffer.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

buffer::buffer(std::shared_ptr<gpu::device> &&device,
			gpu::resource_type resource_type,
			gpu::sync_queues sync_queues,
			gpu::buffer_features features,
			size_t size)
			: gpu::buffer(std::move(device)),
			_device(*static_cast<vk12::device *>(this->device.get())) {
	auto &adapter = *static_cast<vk12::adapter *>(_device.get_adapter().get());

	VkBufferUsageFlags buffer_usage = (features & 0b11) | ((features & 0b111100) << 2);

	// Buffer will never be used in presentation queue
	std::vector<uint32_t> families = adapter.unique_families(sync_queues);
		
	VkBufferCreateInfo buffer_info{};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = buffer_usage;
	buffer_info.sharingMode = (families.size() > 1 ? VK_SHARING_MODE_CONCURRENT : VK_SHARING_MODE_EXCLUSIVE);
	buffer_info.queueFamilyIndexCount = families.size();
	buffer_info.pQueueFamilyIndices = families.data();
	
	VmaAllocationCreateInfo allocation_info = {};
	allocation_info.usage = vk12::vma_memory_usages[resource_type];

	vmaCreateBuffer(_device.allocator, &buffer_info, &allocation_info, &vk_buffer, &allocation, nullptr);
}

buffer::~buffer() {
	vmaDestroyBuffer(_device.allocator, vk_buffer, allocation);
}

void buffer::map(void **ptr) {
	VOLT_VK12_DEBUG_CHECK(vmaMapMemory(_device.allocator, allocation, ptr), "Failed to map buffer memory.")
}

void buffer::unmap() {
	vmaUnmapMemory(_device.allocator, allocation);
}

void buffer::read(size_t offset, size_t size) {
	VOLT_VK12_DEBUG_CHECK(vmaInvalidateAllocation(_device.allocator, allocation, offset, size),  "Failed to invalidate allocation.")
}

void buffer::write(size_t offset, size_t size) {
	VOLT_VK12_DEBUG_CHECK(vmaFlushAllocation(_device.allocator, allocation, offset, size), "Failed to flush allocation.")
}

}
