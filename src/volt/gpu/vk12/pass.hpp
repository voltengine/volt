#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/pass.hpp>

namespace volt::gpu::vk12 {

class rasterization_pass : public gpu::rasterization_pass {
public:
	vk12::routine_impl &impl;
	VkPipelineLayout pipeline_layout;
	VkRenderPass render_pass;
	VkPipeline pipeline;

	rasterization_pass(vk12::routine_impl &impl);

	void draw(uint32_t index_count, uint32_t instance_count = 1) override;

	void index_buffer(uint32_t location, const std::shared_ptr<gpu::buffer> &buffer) override;

	void vertex_buffer(uint32_t location, const std::shared_ptr<gpu::buffer> &buffer) override;
};

class compute_pass : public gpu::compute_pass {
public:
	struct set_layout_create_info {
		VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	vk12::routine_impl &impl;
	std::unordered_map<uint32_t, set_layout_create_info> set_layout_create_infos;
	std::vector<VkDescriptorSetLayout> set_layouts;
	std::unordered_map<std::string, vk12::shader::binding_point> binding_points;
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;

	compute_pass(vk12::routine_impl &impl, const compute_pass_info &info);

	~compute_pass();

	void dispatch(math::uvec3 group_count) override;
};

}
