#pragma once

#include <volt/pch.hpp>

#include <volt/video/fence.hpp>
#include <volt/video/queue.hpp>

namespace volt::video::d3d12 {

template<command_type T>
class queue : public video::queue<T> {
public:
	ID3D12CommandQueue* command_queue;
	std::shared_ptr<fence> wait_fence;
	uint64_t wait_counter = 0;

	queue(std::shared_ptr<video::device> &&device);

	~queue();

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
