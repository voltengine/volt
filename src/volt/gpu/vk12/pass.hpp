#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pass.hpp>
#include <volt/gpu/vk12/graph.hpp>

namespace volt::gpu::vk12 {

class pass : public gpu::pass {
public:
	vk12::graph &graph;

	pass(vk12::graph &graph);

	void draw(const draw_info &info) override;
};

class async_pass : public gpu::async_pass {
public:
	vk12::graph &graph;

	async_pass(vk12::graph &graph);

	void draw(uint32_t thread_index, const draw_info &info) override;
};

}
