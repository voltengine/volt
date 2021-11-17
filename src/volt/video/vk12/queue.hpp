#pragma once

#include <volt/pch.hpp>

#include <volt/video/queue.hpp>

namespace volt::video::vk12 {

// Submit nodes in the pool will preserve capacities of vectors
struct _submit_node {
	std::vector<VkSemaphore> wait_semaphores;
	std::vector<uint64_t> wait_values;

	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkSemaphore> signal_semaphores;
	std::vector<uint64_t> signal_values;

	void clear();
};

template<command_type T>
class queue : public video::queue<T> {
public:
	VkQueue vk_queue;
	std::vector<_submit_node> submit_node_pool;
	size_t active_submit_nodes = 0;
	uint8_t submission_build_stage = 3; // 0 - wait; 1 - execute; 2 - signal
	std::vector<std::pair<VkSubmitInfo, VkTimelineSemaphoreSubmitInfo>> info_pool;

	queue(std::shared_ptr<video::device> &&device, VkQueue vk_queue);

	std::shared_ptr<video::pool<T>> create_pool() override;

	void wait(const std::shared_ptr<video::fence> &fence, uint64_t value) override;

	void execute(const std::shared_ptr<video::routine<T>> &routine) override;

	void signal(const std::shared_ptr<video::fence> &fence, uint64_t value) override;

	void submit() override;

	void wait() override;
};

using graphics_queue = queue<command_type::graphics>;
using compute_queue = queue<command_type::compute>;
using copy_queue = queue<command_type::copy>;

}
