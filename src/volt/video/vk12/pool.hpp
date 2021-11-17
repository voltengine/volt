#pragma once

#include <volt/pch.hpp>

#include <volt/video/pool.hpp>

namespace volt::video::vk12 {

template<command_type T>
class pool : public video::pool<T> {
public:
	VkCommandPool command_pool;
	VkDevice device;

	pool(std::shared_ptr<video::queue<T>> &&queue);

	~pool();

	std::shared_ptr<video::routine<T>> create_routine() override;

	std::shared_ptr<video::subroutine<T>> create_subroutine() override;

	void reset_routines() override;
};

using graphics_pool = pool<command_type::graphics>;
using compute_pool = pool<command_type::compute>;
using copy_pool = pool<command_type::copy>;

}
