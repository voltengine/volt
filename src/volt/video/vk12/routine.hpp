#pragma once

#include <volt/pch.hpp>

#include <volt/video/routine.hpp>

namespace volt::video::vk12 {

template<command_type T, bool Sub = false>
class routine : public video::routine<T, Sub> {
public:
	VkCommandBuffer command_buffer;

	routine(std::shared_ptr<video::pool<T>> &&pool);

	~routine();

	void begin(bool disposable) override;

	// template<bool Subroutine = Sub, std::enable_if_t<Subroutine, std::nullptr_t> = nullptr>
	// void begin(bool disposable, render_pass, uint32_t subpass, framebuffer, bool occlusion_query, bool precise_query) override;
	
	void end() override;

	// void begin_render_pass(); // whether the first subpass is all contained in a routine or subroutine

	void execute_subroutine(const std::shared_ptr<video::routine<T, true>> &subroutine) override;
};

template<command_type T>
using subroutine = routine<T, true>;

using graphics_routine = routine<command_type::graphics>;
using compute_routine = routine<command_type::compute>;
using copy_routine = routine<command_type::copy>;

using graphics_subroutine = subroutine<command_type::graphics>;
using compute_subroutine = subroutine<command_type::compute>;
using copy_subroutine = subroutine<command_type::copy>;

}
