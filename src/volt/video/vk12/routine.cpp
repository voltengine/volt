#include <volt/video/vk12/routine.hpp>

#include <volt/video/vk12/device.hpp>
#include <volt/video/vk12/pool.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12 {

template<command_type T, bool Sub>
routine<T, Sub>::routine(std::shared_ptr<video::pool<T>> &&pool)
		: video::routine<T, Sub>(std::move(pool)) {
	VkCommandPool vk_pool = static_cast<vk12::pool<T> *>(this->pool.get())->command_pool;
	VkDevice device = static_cast<vk12::device *>(this->pool->get_queue()->get_device().get())->vk_device;

	VkCommandBufferAllocateInfo command_buffer_info = {};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.commandPool = vk_pool;
	if constexpr (Sub)
		command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	else
		command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	vkAllocateCommandBuffers(device, &command_buffer_info, &command_buffer);
}

template<command_type T, bool Sub>
routine<T, Sub>::~routine() {
	VkCommandPool vk_pool = static_cast<vk12::pool<T> *>(this->pool.get())->command_pool;
	VkDevice device = static_cast<vk12::device *>(this->pool->get_queue()->get_device().get())->vk_device;
	vkFreeCommandBuffers(device, vk_pool, 1, &command_buffer);
}

template<command_type T, bool Sub>
void routine<T, Sub>::begin(bool disposable) {
	if constexpr (!Sub) {
		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = disposable ? VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT : 0; // continue_bit is set if subroutine + optional disposable bit

		VOLT_VK12_DEBUG_CHECK(vkBeginCommandBuffer(command_buffer, &begin_info),
				"Failed to begin command buffer recording.")
	} else {
		VOLT_ASSERT(false, "routine<T, Sub>::begin(bool disposable) is unavailable to subroutines. Please use the alternative overload of this method.");
	}
}

// template<command_type T, bool Sub>
// template<bool Subroutine = Sub, std::enable_if_t<Subroutine, std::nullptr_t> = nullptr>
// void routine<T, Sub>::begin(bool) {
// 	VkCommandBufferBeginInfo begin_info{};
// 	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
// 	begin_info.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT ; // continue_bit is set if subroutine + optional disposable bit
// 	begin_info.pInheritanceInfo =  // reuired for subroutine

// 	VOLT_VIDEO_DEBUG_ASSERT(vkBeginCommandBuffer(command_buffer, &begin_info)
// 			== VK_SUCCESS, "Failed to begin command buffer recording.")
// }
	
template<command_type T, bool Sub>
void routine<T, Sub>::end() {
	VOLT_VK12_DEBUG_CHECK(vkEndCommandBuffer(command_buffer),
			"Failed to end command buffer recording.")
}

template<command_type T, bool Sub>
void routine<T, Sub>::execute_subroutine(const std::shared_ptr<video::subroutine<T>> &subroutine) {
	if constexpr (!Sub) {
		VkCommandBuffer subroutine_command_buffer = static_cast<vk12::
				subroutine<T> *>(subroutine.get())->command_buffer;
		vkCmdExecuteCommands(command_buffer, 1, &subroutine_command_buffer);
	} else {
		VOLT_ASSERT(false, "routine<T, Sub>::execute_subroutine(...) is unavailable to subroutines. Only routines can execute subroutines.");
	}
}

template class routine<command_type::graphics>;
template class routine<command_type::compute>;
template class routine<command_type::copy>;

template class routine<command_type::graphics, true>;
template class routine<command_type::compute, true>;
template class routine<command_type::copy, true>;

}