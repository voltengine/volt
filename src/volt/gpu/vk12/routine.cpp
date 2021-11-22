#include <volt/gpu/vk12/routine.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/pool.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

template<command_types T>
routine<T>::routine(std::shared_ptr<gpu::pool<T>> &&pool)
		: gpu::routine<T>(std::move(pool)) {
	VkCommandPool vk_pool = static_cast<vk12::pool<T> *>(this->pool.get())->command_pool;
	VkDevice device = static_cast<vk12::device *>(this->pool->get_queue()->get_device().get())->vk_device;

	VkCommandBufferAllocateInfo command_buffer_info = {};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.commandPool = vk_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &command_buffer_info, &command_buffer);
}

template<command_types T>
routine<T>::~routine() {
	VkCommandPool vk_pool = static_cast<vk12::pool<T> *>(this->pool.get())->command_pool;
	VkDevice device = static_cast<vk12::device *>(this->pool->get_queue()->get_device().get())->vk_device;
	vkFreeCommandBuffers(device, vk_pool, 1, &command_buffer);
}

template<command_types T>
void routine<T>::begin() {
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VOLT_VK12_DEBUG_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info),
			"Failed to begin command buffer recording.")
}
	
template<command_types T>
void routine<T>::end() {
	VOLT_VK12_DEBUG_CHECK(vkEndCommandBuffer(command_buffer),
			"Failed to end command buffer recording.")
}

template class routine<command_type::rasterization>;
template class routine<command_type::compute>;
template class routine<command_type::copy>;

}