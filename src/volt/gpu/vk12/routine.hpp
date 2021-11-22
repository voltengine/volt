#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/routine.hpp>

namespace volt::gpu::vk12 {

template<command_types T>
class routine : public gpu::routine<T> {
public:
	VkCommandBuffer command_buffer;

	routine(std::shared_ptr<gpu::pool<T>> &&pool);

	~routine();

	void begin() override;

	void end() override;
};

using rasterization_routine = routine<command_type::rasterization>;
using compute_routine = routine<command_type::compute>;
using copy_routine = routine<command_type::copy>;

}
