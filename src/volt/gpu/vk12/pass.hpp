#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pass.hpp>

namespace volt::gpu::vk12 {

class rasterization_pass : public gpu::rasterization_pass {
public:
	VkPipelineLayout pipeline_layout;
	VkRenderPass render_pass;
	VkPipeline pipeline;

	void index_buffer(uint32_t location, std::shared_ptr<gpu::buffer> &&buffer) override;

	void vertex_buffer(uint32_t location, std::shared_ptr<gpu::buffer> &&buffer) override;

	void sampler(std::string_view slot, std::shared_ptr<gpu::texture> &&texture) override;

	void constant_buffer(std::string_view slot, std::shared_ptr<gpu::buffer> &&buffer) override;

	void draw(uint32_t index_count, uint32_t instance_count = 1) override;
};

}
