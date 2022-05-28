#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/enums.hpp>
#include <volt/gpu/pass.hpp>
#include <volt/gpu/texture.hpp>
#include <volt/gpu/vk12/shader.hpp>

namespace volt::gpu::vk12 {

struct compute_pipeline_key {
	gpu::shader *compute_shader;
};

struct render_pass_key {
	std::vector<VkFormat> color_attachment_formats;
	VkFormat depth_stencil_attachment_format = VK_FORMAT_UNDEFINED;
	std::vector<gpu::attachment_initializer> color_attachment_initializers;
	gpu::attachment_initializer depth_stencil_attachment_initializer;
};

struct rasterization_pipeline_key {
	VkRenderPass render_pass;
	gpu::shader *vertex_shader;
	gpu::shader *hull_shader;
	gpu::shader *domain_shader;
	gpu::shader *geometry_shader;
	gpu::shader *pixel_shader;
	std::unordered_set<std::string> instance_inputs;
	gpu::topology polygon_mode;
	gpu::topology primitive_mode;
	bool culling;
	bool depth_test, depth_write, stencil_test, stencil_write;
	gpu::blending blending;
};

struct framebuffer_key {
	VkRenderPass render_pass;
	std::vector<gpu::texture *> color_attachments;
	gpu::texture *depth_stencil_attachment;
};

struct pipeline {
	std::vector<VkDescriptorSetLayout> set_layouts;
	std::unordered_map<std::string, vk12::shader::binding_point> binding_points;
	VkPipelineLayout pipeline_layout;
	VkPipeline vk_pipeline;
};

bool operator==(const compute_pipeline_key &lhs, const compute_pipeline_key &rhs);

bool operator==(const render_pass_key &lhs, const render_pass_key &rhs);

bool operator==(const rasterization_pipeline_key &lhs, const rasterization_pipeline_key &rhs);

bool operator==(const framebuffer_key &lhs, const framebuffer_key &rhs);

}

namespace std {

template<>
struct hash<volt::gpu::vk12::compute_pipeline_key> {
	size_t operator()(const volt::gpu::vk12::compute_pipeline_key &key) const;
};

template<>
struct hash<volt::gpu::vk12::render_pass_key> {
	size_t operator()(const volt::gpu::vk12::render_pass_key &key) const;
};

template<>
struct hash<volt::gpu::vk12::rasterization_pipeline_key> {
	size_t operator()(const volt::gpu::vk12::rasterization_pipeline_key &key) const;
};

template<>
struct hash<volt::gpu::vk12::framebuffer_key> {
	size_t operator()(const volt::gpu::vk12::framebuffer_key &key) const;
};

}

namespace volt::gpu::vk12 {

class device;

class jit {
public:
	jit(vk12::device &device);

	~jit();

	// Thread safe methods (they only lock when initializing)

	pipeline &compute_pipeline(compute_pipeline_key &&key);

	VkRenderPass render_pass(render_pass_key &&key);

	pipeline &rasterization_pipeline(rasterization_pipeline_key &&key);

	VkFramebuffer framebuffer(framebuffer_key &&key);

	// End of thread safe methods

	void optimize();

	void clear_framebuffers();

	void clear();

private:
	struct cache {
		std::unordered_map<compute_pipeline_key, pipeline> compute_pipelines;
		std::unordered_map<render_pass_key, VkRenderPass> render_passes;
		std::unordered_map<rasterization_pipeline_key, pipeline> rasterization_pipelines;
		std::unordered_map<framebuffer_key, VkFramebuffer> framebuffers;
	};

	vk12::device &device;
	cache immutable_cache;
	cache mutable_cache;
	std::mutex mutable_cache_mutex;

	template<typename Key>
	pipeline create_pipeline(Key &key, const std::vector<vk12::shader *> &shaders);
};

}
