#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pass_graph.hpp>

namespace volt::gpu::vk12 {

class pass_graph : public gpu::pass_graph {
public:
	pass_graph(std::shared_ptr<gpu::device> &&device);

	~pass_graph();

	void execute() override;

private:
	struct built_pass {
		VkRenderPass render_pass;
		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;
		VkFramebuffer framebuffer; // need multiple for the final pass
	};

	VkDevice vk_device;
	std::vector<built_pass> built_passes;

	void build() override;
};

}
