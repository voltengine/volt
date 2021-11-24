#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pool.hpp>

namespace volt::gpu::vk12 {

template<command_types T>
class pool : public gpu::pool<T> {
public:
	VkCommandPool command_pool;

	pool(std::shared_ptr<gpu::device> &&device);

	~pool();

	std::shared_ptr<gpu::routine<T>> create_routine() override;

	void reset_routines() override;

private:
	VkDevice vk_device;
};

using rasterization_pool = pool<command_type::rasterization>;
using compute_pool = pool<command_type::compute>;
using copy_pool = pool<command_type::copy>;

}
