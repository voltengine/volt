#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/queue.hpp>

namespace volt::gpu::vk12 {

// Submit nodes in the pool will preserve capacities of vectors
struct _submit_node {
	std::vector<VkSemaphore> wait_semaphores;
	std::vector<uint64_t> wait_values;

	std::vector<VkCommandBuffer> command_buffers;

	std::vector<VkSemaphore> signal_semaphores;
	std::vector<uint64_t> signal_values;

	void clear();
};

template<command_types T>
class queue : public gpu::queue<T> {
public:
	VkQueue vk_queue;
	std::vector<_submit_node> submit_node_pool;
	size_t active_submit_nodes = 0;
	uint8_t submission_build_stage = 3; // 0 - wait; 1 - execute; 2 - signal
	std::vector<VkSubmitInfo> submit_info_pool;
	std::vector<VkTimelineSemaphoreSubmitInfo> timeline_info_pool;

	queue(std::shared_ptr<gpu::device> &&device, VkQueue vk_queue);

	std::shared_ptr<gpu::pool<T>> create_pool() override;

	void wait(const std::shared_ptr<gpu::fence> &fence, uint64_t value) override;

	void execute(const std::shared_ptr<gpu::routine<T>> &routine) override;

	void signal(const std::shared_ptr<gpu::fence> &fence, uint64_t value) override;

	void flush() override;

	void wait() override;
};

using rasterization_queue = queue<command_type::rasterization>;
using compute_queue = queue<command_type::compute>;
using copy_queue = queue<command_type::copy>;

}
