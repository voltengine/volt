#include <volt/gpu/vk12/queue.hpp>

#include <volt/gpu/vk12/fence.hpp>
#include <volt/gpu/vk12/pool.hpp>
#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

void _submit_node::clear() {
	wait_semaphores.clear();
	wait_values.clear();
	command_buffers.clear();
	signal_semaphores.clear();
	signal_values.clear();
}

template<command_types T>
queue<T>::queue(std::shared_ptr<gpu::device> &&device, VkQueue vk_queue)
		: gpu::queue<T>(std::move(device)), vk_queue(vk_queue) {}

template<command_types T>
std::shared_ptr<gpu::pool<T>> queue<T>::create_pool() {
	return std::shared_ptr<gpu::pool<T>>(
			new vk12::pool<T>(queue<T>::shared_from_this()));
}

template<command_types T>
void queue<T>::wait(const std::shared_ptr<gpu::fence> &fence, uint64_t value) {
	if (submission_build_stage > 0) {
		if (submit_node_pool.size() == active_submit_nodes)
			submit_node_pool.emplace_back();
		else
			submit_node_pool[active_submit_nodes].clear();
		active_submit_nodes++;
	}
	submission_build_stage = 0;

	_submit_node &node = submit_node_pool[active_submit_nodes - 1];
	node.wait_semaphores.push_back(static_cast<vk12::fence *>(fence.get())->semaphore);
	node.wait_values.push_back(value);
}

template<command_types T>
void queue<T>::execute(const std::shared_ptr<gpu::routine<T>> &routine) {
	if (submission_build_stage > 1) {
		if (submit_node_pool.size() == active_submit_nodes)
			submit_node_pool.emplace_back();
		else
			submit_node_pool[active_submit_nodes].clear();
		active_submit_nodes++;
	}
	submission_build_stage = 1;
	
	_submit_node &node = submit_node_pool[active_submit_nodes - 1];
	node.command_buffers.push_back(static_cast<vk12::routine<T> *>(routine.get())->command_buffer);
}

template<command_types T>
void queue<T>::signal(const std::shared_ptr<gpu::fence> &fence, uint64_t value) {
	if (submission_build_stage > 2) {
		if (submit_node_pool.size() == active_submit_nodes)
			submit_node_pool.emplace_back();
		else
			submit_node_pool[active_submit_nodes].clear();
		active_submit_nodes++;
	}
	submission_build_stage = 2;

	_submit_node &node = submit_node_pool[active_submit_nodes - 1];
	node.signal_semaphores.push_back(static_cast<vk12::fence *>(fence.get())->semaphore);
	node.signal_values.push_back(value);
}

template<command_types T>
void queue<T>::flush() {
	if (active_submit_nodes == 0)
		return;

	if (active_submit_nodes > submit_info_pool.size()) {
		submit_info_pool.resize(active_submit_nodes,
				{ VK_STRUCTURE_TYPE_SUBMIT_INFO });
		timeline_info_pool.resize(active_submit_nodes,
				{ VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO });
		submit_info_pool.back().pNext = &timeline_info_pool.back();
	}

	for (size_t i = 0; i < active_submit_nodes; i++) {
		_submit_node &node = submit_node_pool[i];

		VkSubmitInfo &submit_info = submit_info_pool[i];
		VkTimelineSemaphoreSubmitInfo &timeline_info = timeline_info_pool[i];

		submit_info.waitSemaphoreCount   = node.wait_semaphores.size();
		submit_info.pWaitSemaphores      = node.wait_semaphores.data();
		submit_info.signalSemaphoreCount = node.signal_semaphores.size();
		submit_info.pSignalSemaphores    = node.signal_semaphores.data();
		submit_info.commandBufferCount   = node.command_buffers.size();
		submit_info.pCommandBuffers      = node.command_buffers.data();

		timeline_info.waitSemaphoreValueCount   = node.wait_values.size();
		timeline_info.pWaitSemaphoreValues      = node.wait_values.data();
		timeline_info.signalSemaphoreValueCount = node.signal_values.size();
		timeline_info.pSignalSemaphoreValues    = node.signal_values.data();
	}

	VOLT_VK12_DEBUG_CHECK(vkQueueSubmit(vk_queue, active_submit_nodes, submit_info_pool.data(), VK_NULL_HANDLE),
			"Failed to submit queue");
	
	active_submit_nodes = 0;
	submission_build_stage = 3;
}

template<command_types T>
void queue<T>::wait() {
	VOLT_VK12_DEBUG_CHECK(vkQueueWaitIdle(vk_queue),
			"Failed to wait for queue.")
}

template class queue<command_type::rasterization>;
template class queue<command_type::compute>;
template class queue<command_type::copy>;

}
