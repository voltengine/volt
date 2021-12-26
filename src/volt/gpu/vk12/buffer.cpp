#include <volt/gpu/vk12/buffer.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

buffer::buffer(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::buffer_features features,
			size_t size)
			: gpu::buffer(std::move(device), size),
			vk12_device(*static_cast<vk12::device *>(this->_device.get())) {
	auto &adapter = *static_cast<vk12::adapter *>(_device->adapter().get());

	VkBufferCreateInfo buffer_info{};
	buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_info.size = size;
	buffer_info.usage = (features & 0b11) | ((features & 0b111100) << 2);
	buffer_info.sharingMode = adapter.unique_families.size() == 1 ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT; // Force EXCLUSIVE + single queue on mobile
	buffer_info.queueFamilyIndexCount = adapter.unique_families.size();
	buffer_info.pQueueFamilyIndices = adapter.unique_families.data();
	
	VmaAllocationCreateInfo allocation_info = {};
	allocation_info.usage = vk12::vma_memory_usages[memory_type];

	vmaCreateBuffer(vk12_device.allocator, &buffer_info, &allocation_info, &vk_buffer, &allocation, nullptr);
}

buffer::~buffer() {
	vmaDestroyBuffer(vk12_device.allocator, vk_buffer, allocation);
}

void buffer::map(void **ptr, size_t read_offset, size_t read_size) {
	VOLT_VK12_DEBUG_CHECK(vmaMapMemory(vk12_device.allocator, allocation, ptr),
			"Failed to map memory.");
	
	if (read_size != 0)
		VOLT_VK12_DEBUG_CHECK(vmaInvalidateAllocation(vk12_device.allocator, allocation, read_offset, read_size),
				"Failed to invalidate allocation.");
}

void buffer::unmap(size_t write_offset, size_t write_size) {
	vmaUnmapMemory(vk12_device.allocator, allocation);

	if (write_size != 0)
		VOLT_VK12_DEBUG_CHECK(vmaFlushAllocation(vk12_device.allocator, allocation, write_offset, write_size),
				"Failed to flush allocation.");
}

void buffer::barrier(vk12::routine_impl &impl, state state) {
	access_pattern access_pattern = access_patterns[state];

	// Some access types do not need synchronization
	if ((state == buffer::state::copy_src
			|| state == buffer::state::vertex_input
			|| state == buffer::state::index_input
			|| state == buffer::state::rasterization_shared
			|| state == buffer::state::compute_shared) && // read -> read
			(current_state == buffer::state::copy_src
			|| current_state == buffer::state::vertex_input
			|| current_state == buffer::state::index_input
			|| current_state == buffer::state::rasterization_shared
			|| current_state == buffer::state::compute_shared)) {
		current_access_pattern.stage_mask |= access_pattern.stage_mask;
		current_access_pattern.access_mask |= access_pattern.access_mask;
		return;
	}
	
	VkBufferMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcAccessMask = current_access_pattern.access_mask;
	barrier.dstAccessMask = access_pattern.access_mask;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.buffer = vk_buffer;
	barrier.offset = 0;
	barrier.size = VK_WHOLE_SIZE;

	vkCmdPipelineBarrier(impl.command_buffer,
    		current_access_pattern.stage_mask, access_pattern.stage_mask,
    		0, 0, nullptr, 1, &barrier, 0, nullptr);

	current_access_pattern = access_pattern;

	if (current_routine_impl != &impl && current_routine_impl != nullptr) {
		impl.wait_semaphores.insert({ current_routine_impl->finish_semaphore, access_pattern.stage_mask, current_routine_impl->finish_value });
		current_routine_impl = &impl;
	}
}

std::unordered_map<vk12::buffer::state, buffer::access_pattern> buffer::access_patterns{
	{ vk12::buffer::state::undefined, { VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_ACCESS_MEMORY_WRITE_BIT } },
	{ vk12::buffer::state::copy_src, { VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_READ_BIT } },
	{ vk12::buffer::state::copy_dst, { VK_PIPELINE_STAGE_TRANSFER_BIT, VK_ACCESS_TRANSFER_WRITE_BIT } },
	{ vk12::buffer::state::index_input, { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_INDEX_READ_BIT } },
	{ vk12::buffer::state::vertex_input, { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT } },
	{ vk12::buffer::state::rasterization_shared, { VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT } },
	{ vk12::buffer::state::rasterization_unique, { VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT } },
	{ vk12::buffer::state::compute_shared, { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT } },
	{ vk12::buffer::state::compute_unique, { VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT } }
};

}
