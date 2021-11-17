#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/fence.hpp>
#include <volt/gpu/queue.hpp>

namespace volt::gpu::d3d12 {

template<command_type T>
class queue : public gpu::queue<T> {
public:
	ID3D12CommandQueue* command_queue;
	std::vector<ID3D12CommandList *> command_lists;
	std::shared_ptr<fence> wait_fence;
	uint64_t wait_counter = 0;

	queue(std::shared_ptr<gpu::device> &&device);

	~queue();

	std::shared_ptr<gpu::pool<T>> create_pool() override;

	void wait(const std::shared_ptr<gpu::fence> &fence, uint64_t value) override;

	void execute(const std::shared_ptr<gpu::routine<T>> &routine) override;

	void signal(const std::shared_ptr<gpu::fence> &fence, uint64_t value) override;

	void flush() override;

	void wait() override;
};

using graphics_queue = queue<command_type::graphics>;
using compute_queue = queue<command_type::compute>;
using copy_queue = queue<command_type::copy>;

}
