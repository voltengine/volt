#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pass.hpp>
#include <volt/gpu/vk12/routine.hpp>

namespace volt::gpu::vk12 {

class pass_context : public gpu::pass_context {
public:
	vk12::routine_impl &impl;

	pass_context(vk12::routine_impl &impl);

	void draw(const draw_info &info) override;
};

class async_pass_context : public gpu::async_pass_context {
public:
	vk12::routine_impl &impl;

	async_pass_context(vk12::routine_impl &impl);

	void draw(uint32_t thread_index, const draw_info &info) override;
};

}
