#include <volt/video/vk12/queue.hpp>

#include <volt/video/vk12/pool.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::video::vk12 {

void _submit_node::clear() {
	wait_semaphores.clear();
	wait_values.clear();
	command_buffers.clear();
	signal_semaphores.clear();
	signal_values.clear();
}

template<command_type T>
queue<T>::queue(std::shared_ptr<video::device> &&device, VkQueue vk_queue)
		: video::queue<T>(std::move(device)), vk_queue(vk_queue) {}

template<command_type T>
std::shared_ptr<video::pool<T>> queue<T>::create_pool() {
	return std::shared_ptr<video::pool<T>>(
			new vk12::pool<T>(queue<T>::shared_from_this()));
}

template<command_type T>
void queue<T>::wait(const std::shared_ptr<video::fence> &fence, uint64_t value) {
	if (submission_build_stage > 0) {
		if (submit_node_pool.size() == active_submit_nodes)
			submit_node_pool.emplace_back();
		else
			submit_nodes[active_submit_nodes].clear();
		active_submit_nodes++
	}
	submission_build_stage = 0;

	submit_node &node = submit_node_pool[active_submit_nodes - 1];
	node.wait_semaphores.push_back(static_cast<vk12::fence *>(fence.get())->semaphore);
	node.wait_values.push_back(value);
}

template<command_type T>
void queue<T>::execute(const std::shared_ptr<video::routine<T>> &routine) {
	if (submission_build_stage > 1) {
		if (submit_node_pool.size() == active_submit_nodes)
			submit_node_pool.emplace_back();
		else
			submit_nodes[active_submit_nodes].clear();
		active_submit_nodes++
	}
	submission_build_stage = 1;
	
	_submit_node &node = submit_node_pool[active_submit_nodes - 1];
	node.command_buffers.push_back(static_cast<vk12::routine *>(routine.get())->command_buffer);
}

template<command_type T>
void queue<T>::signal(const std::shared_ptr<video::fence> &fence, uint64_t value) {
	if (submission_build_stage > 2) {
		if (submit_node_pool.size() == active_submit_nodes)
			submit_node_pool.emplace_back();
		else
			submit_nodes[active_submit_nodes].clear();
		active_submit_nodes++
	}
	submission_build_stage = 2;

	_submit_node &node = submit_node_pool[active_submit_nodes - 1];
	node.signal_semaphores.push_back(static_cast<vk12::fence *>(fence.get())->semaphore);
	node.signal_values.push_back(value);
}

template<command_type T>
void queue<T>::submit() {
	info_pool.reserve(active_submit_nodes);

	for (size_t i = 0; i < active_submit_nodes; i++) {
		_submit_node &node = submit_node_pool[i];

		VkSubmitInfo &submit_info = info_pool[i].first;
		VkTimelineSemaphoreSubmitInfo &timeline_info = info_pool[i].second;

		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.pNext = &timeline_info;
		submit_info.waitSemaphoreCount   = node.wait_semaphores.size();
		submit_info.pWaitSemaphores      = node.wait_semaphores.data();
		submit_info.signalSemaphoreCount = node.signal_semaphores.size();
		submit_info.pSignalSemaphores    = node.signal_semaphores.data();
		submit_info.commandBufferCount   = node.command_buffers.size();
		submit_info.pCommandBuffers      = node.command_buffers.data();

		timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
		timeline_info.pNext = nullptr;
		timeline_info.waitSemaphoreValueCount   = node.wait_values.size();
		timeline_info.pWaitSemaphoreValues      = node.wait_values.data();
		timeline_info.signalSemaphoreValueCount = node.signal_values.size();
		timeline_info.pSignalSemaphoreValues    = node.signal_values.data();
	}

	VOLT_VK12_DEBUG_CHECK(vkQueueSubmit(vk_queue, active_submit_nodes, info_pool.data(), VK_NULL_HANDLE),
			"Failed to submit queue");
	active_submit_nodes = 0;
	submission_build_stage = 3;
}

template<command_type T>
void queue<T>::wait() {
	VOLT_VK12_DEBUG_CHECK(vkQueueWaitIdle(vk_queue),
			"Failed to wait for queue.")
}

template class queue<command_type::graphics>;
template class queue<command_type::compute>;
template class queue<command_type::copy>;

}
