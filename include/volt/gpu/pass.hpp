#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "../math/math.hpp"
#include "enums.hpp"
#include "buffer.hpp"
#include "sampler.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device;

#pragma region Pass information structures

class ray_tracing_pass_info;

class viewport {
public:
	math::fvec2 width, height, depth = { 0, 1 };
};

class rasterization_pass_info {
public:
	class color_attachment_info {
	public:
		std::shared_ptr<gpu::texture> texture;
		attachment_initializer initializer = attachment_initializer::none;
		math::fvec3 clear_color = math::fvec3::zero;

		bool operator==(const color_attachment_info &other) const;
	};

	class depth_stencil_attachment_info {
	public:
		std::shared_ptr<gpu::texture> texture;
		attachment_initializer initializer = attachment_initializer::none;
		float clear_depth = 1;
		float clear_stencil = 0;
	};

	const std::shared_ptr<gpu::shader> vertex_shader; // Required
	const std::shared_ptr<gpu::shader> hull_shader;
	const std::shared_ptr<gpu::shader> domain_shader;
	const std::shared_ptr<gpu::shader> geometry_shader;
	const std::shared_ptr<gpu::shader> pixel_shader; // Required
	const std::unordered_set<std::string> instance_inputs; // Others will become vertex inputs
	const gpu::topology primitive_mode = gpu::topology::triangles; // How vertices are interconnected
	const gpu::topology polygon_mode = gpu::topology::triangles; // How polygons are displayed when primitive_mode == triangles
	const bool culling = true;
	const float line_width = 1;
	const bool depth_test = false, depth_write = false, stencil_test = false, stencil_write = false;
	const gpu::blending blending = gpu::blending::alpha;
	const std::vector<color_attachment_info> color_attachments;
	const depth_stencil_attachment_info depth_stencil_attachment;
};

class compute_pass_info {
public:
	const std::shared_ptr<gpu::shader> compute_shader;
};

#pragma endregion

class _base_pass {
public:
	// Those are raw pointers so any bound resources must not be destroyed until dispatch
	std::unordered_map<std::string, gpu::buffer *> _constant_buffers;
	std::unordered_map<std::string, std::pair<gpu::texture *, gpu::sampler *>> _sampled_textures;
	std::unordered_map<std::string, std::pair<gpu::buffer *, bool>> _storage_buffers;
	std::unordered_map<std::string, std::pair<gpu::texture *, bool>> _storage_textures;

	// Readonly access; Guarantees at most 16 KiB to be available
	VOLT_API void constant_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer);

	VOLT_API void sampled_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture, const std::shared_ptr<gpu::sampler> &sampler);

	// Large data; Optionally writeable; Slower on some hardware
	VOLT_API void storage_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer, bool shared);

	VOLT_API void storage_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture, bool shared);

	VOLT_API void reset();

	VOLT_API const std::shared_ptr<gpu::device> &device();

protected:
	std::shared_ptr<gpu::device> _device;

	VOLT_API _base_pass(std::shared_ptr<gpu::device> &&device);
};

// class ray_tracing_pass;

class rasterization_pass : public _base_pass {
public:
	virtual ~rasterization_pass() = default;

	virtual void draw(uint32_t index_count, uint32_t instance_count = 1) = 0;

	virtual void index_buffer(const std::shared_ptr<gpu::buffer> &buffer) = 0;

	// Vertex buffer that advances for each vertex
	virtual void vertex_buffer(const std::shared_ptr<gpu::buffer> &buffer) = 0;

	// Vertex buffer that advances for each instance
	virtual void instance_buffer(const std::shared_ptr<gpu::buffer> &buffer) = 0; 

	virtual void viewport(gpu::viewport viewport) = 0;

protected:
	rasterization_pass(std::shared_ptr<gpu::device> &&device);
};

class compute_pass : public _base_pass {
public:
	virtual ~compute_pass() = default;

	virtual void dispatch(math::uvec3 group_count) = 0;

protected:	
	compute_pass(std::shared_ptr<gpu::device> &&device);
};

}
