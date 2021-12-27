#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../math/math.hpp"
#include "../util/util.hpp"
#include "enums.hpp"
#include "buffer.hpp"
#include "sampler.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace volt::gpu {

struct color_attachment_info {
	util::optional_shared_ptr_ref<gpu::texture> texture;
	attachment_initializer initializer = attachment_initializer::none;
	math::fvec4 clear_color = math::fvec4::zero;
};

struct depth_stencil_attachment_info {
	util::optional_shared_ptr_ref<gpu::texture> texture;
	attachment_initializer initializer = attachment_initializer::none;
	float clear_depth = 1;
	bool clear_stencil = false;
};

struct pass_info {
	std::vector<color_attachment_info> color_attachments;
	std::optional<depth_stencil_attachment_info> depth_stencil_attachment;
};

struct constant_buffer_binding {
	std::string slot;
	util::optional_shared_ptr_ref<gpu::buffer> buffer;
};

struct sampled_texture_binding {
	std::string slot;
	util::optional_shared_ptr_ref<gpu::texture> texture;
	util::optional_shared_ptr_ref<gpu::sampler> sampler;
};

struct storage_buffer_binding {
	std::string slot;
	util::optional_shared_ptr_ref<gpu::buffer> buffer;
	bool shared;
};

struct storage_texture_binding {
	std::string slot;
	util::optional_shared_ptr_ref<gpu::texture> texture;
	bool shared;
};

struct viewport {
	math::fvec2 width, height, depth = { 0, 1 };

	bool operator!=(const viewport &other) const {
		return math::all(width != other.width)
				|| math::all(height != other.height)
				|| math::all(depth != other.depth);
	}
};

struct draw_info {
	// Bindings
	std::vector<constant_buffer_binding> constant_buffers;
	std::vector<sampled_texture_binding> sampled_textures;
	std::vector<storage_buffer_binding> storage_buffers;
	std::vector<storage_texture_binding> storage_textures;
	
	// Shaders
	util::optional_shared_ptr_ref<gpu::shader> vertex_shader; // Required
	util::optional_shared_ptr_ref<gpu::shader> hull_shader;
	util::optional_shared_ptr_ref<gpu::shader> domain_shader;
	util::optional_shared_ptr_ref<gpu::shader> geometry_shader;
	util::optional_shared_ptr_ref<gpu::shader> pixel_shader; // Required

	// Pipeline State
	viewport viewport; // Required
	float line_width = 1;
	gpu::topology primitive_mode = gpu::topology::triangles; // How vertices are interconnected
	gpu::topology polygon_mode = gpu::topology::triangles; // How polygons are displayed when primitive_mode == triangles
	bool culling = true;
	bool depth_test = false, depth_write = false, stencil_test = false, stencil_write = false;
	gpu::blending blending = gpu::blending::alpha;

	// Draw Info
	std::unordered_set<std::string> instance_inputs; // Other inputs will become vertex inputs
	util::optional_shared_ptr_ref<gpu::buffer> index_buffer;
	util::optional_shared_ptr_ref<gpu::buffer> vertex_buffer;
	util::optional_shared_ptr_ref<gpu::buffer> instance_buffer;
	uint32_t draw_count; // Required
	uint32_t instance_count = 1;
};

class pass_context {
public:
	virtual void draw(const draw_info &info) = 0;

protected:
	pass_context() = default;
};

class async_pass_context {
public:
	util::thread_pool &thread_pool;

	virtual void draw(uint32_t thread_index, const draw_info &info) = 0;

protected:
	VOLT_API async_pass_context(util::thread_pool &thread_pool);
};

}
