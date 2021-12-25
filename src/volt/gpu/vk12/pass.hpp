#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/pass.hpp>

namespace volt::gpu::vk12 {

struct pass_set_layout_info {
	VkDescriptorSetLayoutCreateInfo info = { VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	std::vector<VkDescriptorSetLayoutBinding> bindings;
};

class rasterization_pass : public gpu::rasterization_pass {
public:
	std::unordered_map<uint32_t, pass_set_layout_info> pass_set_layout_infos;
	std::vector<VkDescriptorSetLayout> set_layouts;
	std::unordered_map<std::string, vk12::shader::binding_point> binding_points;
	VkPipelineLayout pipeline_layout;
	VkRenderPass render_pass;
	VkPipeline pipeline;
	const std::vector<std::shared_ptr<vk12::texture>> color_attachments;
	const std::shared_ptr<vk12::texture> depth_stencil_attachment;

	rasterization_pass(const rasterization_pass_info &info);

	~rasterization_pass();

	void draw(uint32_t index_count, uint32_t instance_count = 1) override;

	void index_buffer(const std::shared_ptr<gpu::buffer> &buffer) override;

	void vertex_buffer(const std::shared_ptr<gpu::buffer> &buffer) override;

	void instance_buffer(const std::shared_ptr<gpu::buffer> &buffer) override;

	void viewport(gpu::viewport viewport) override;
};

class compute_pass : public gpu::compute_pass {
public:
	std::unordered_map<uint32_t, pass_set_layout_info> pass_set_layout_infos;
	std::vector<VkDescriptorSetLayout> set_layouts;
	std::unordered_map<std::string, vk12::shader::binding_point> binding_points;
	VkPipelineLayout pipeline_layout;
	VkPipeline pipeline;

	compute_pass(const compute_pass_info &info);

	~compute_pass();

	void dispatch(math::uvec3 group_count) override;
};

}
