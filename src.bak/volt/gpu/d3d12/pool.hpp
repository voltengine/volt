#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pool.hpp>

namespace volt::gpu::d3d12 {

template<command_types T>
class pool : public gpu::pool<T> {
public:
	ID3D12CommandAllocator *allocator;

	pool(std::shared_ptr<gpu::queue<T>> &&queue);

	~pool();

	std::shared_ptr<gpu::routine<T>> create_routine() override;

	void reset_routines() override;
};

using rasterization_pool = pool<command_type::rasterization>;
using compute_pool = pool<command_type::compute>;
using copy_pool = pool<command_type::copy>;

}
