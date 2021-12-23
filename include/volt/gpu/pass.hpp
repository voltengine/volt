#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "../math/math.hpp"
#include "enums.hpp"
#include "buffer.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace volt::gpu {

#pragma region Pass information structures

class ray_tracing_pass_info;

class rasterization_pass_info {
public:
	struct _viewport {
		math::fvec2 width, height, depth;
	};

	struct _color_attachment {
		std::shared_ptr<gpu::texture> texture;
		attachment_initializer initializer;
		math::fvec3 clear_color;
	};

	struct _depth_stencil_attachment {
		std::shared_ptr<gpu::texture> texture;
		attachment_initializer initializer;
		float clear_depth;
		float clear_stencil;
	};

	_viewport _viewport;
	std::shared_ptr<gpu::shader> _vertex_shader, _pixel_shader;
	std::unordered_map<uint32_t, _color_attachment> _color_attachments;
	_depth_stencil_attachment _depth_stencil_attachment;

	VOLT_API rasterization_pass_info &viewport(math::fvec2 width, math::
			fvec2 height, math::fvec2 depth = math::fvec2(0, 1));

	VOLT_API rasterization_pass_info &vertex_shader(std::shared_ptr<gpu::shader> shader);

	VOLT_API rasterization_pass_info &pixel_shader(std::shared_ptr<gpu::shader> shader);

	VOLT_API rasterization_pass_info &color_attachment(uint32_t location,
			std::shared_ptr<gpu::texture> texture,
			attachment_initializer initializer,
			math::fvec3 clear_color = math::fvec3::zero);

	VOLT_API rasterization_pass_info &depth_stencil_attachment(
			std::shared_ptr<gpu::texture> texture,
			attachment_initializer initializer,
			float clear_depth = 1,
			float clear_stencil = 0);
};

class compute_pass_info {
public:
	std::shared_ptr<gpu::shader> _compute_shader;

	auto operator<=>(const compute_pass_info &other) const = default;

	VOLT_API compute_pass_info &compute_shader(std::shared_ptr<gpu::shader> shader);
};

#pragma endregion

class _base_pass {
public:
	// Readonly access; Guarantees at most 16 KiB to be available
	VOLT_API void constant_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer);

	VOLT_API void sampled_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture);

	// Large data; Optionally writeable; Slower on some hardware
	VOLT_API void storage_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer, bool shared);

	VOLT_API void storage_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture, bool shared);

	VOLT_API void reset();

protected:
	// Those are raw pointers so any bound resources must not be destroyed until dispatch
	std::unordered_map<std::string, gpu::buffer *> constant_buffers;
	std::unordered_map<std::string, gpu::texture *> sampled_textures;
	std::unordered_map<std::string, std::pair<gpu::buffer *, bool>> storage_buffers;
	std::unordered_map<std::string, std::pair<gpu::texture *, bool>> storage_textures;
};

// class ray_tracing_pass;

class rasterization_pass : public _base_pass {
public:
	virtual ~rasterization_pass() = default;

	virtual void draw(uint32_t index_count, uint32_t instance_count = 1) = 0;

	virtual void index_buffer(uint32_t location, const std::shared_ptr<gpu::buffer> &buffer) = 0;

	virtual void vertex_buffer(uint32_t location, const std::shared_ptr<gpu::buffer> &buffer) = 0;

protected:
	rasterization_pass() = default;
};

class compute_pass : public _base_pass {
public:
	virtual ~compute_pass() = default;

	virtual void dispatch(math::uvec3 group_count) = 0;

protected:	
	compute_pass() = default;
};

}

namespace std {

template<>
struct hash<volt::gpu::compute_pass_info> {
	std::size_t operator()(const volt::gpu::compute_pass_info& info) const {
		return reinterpret_cast<size_t>(info._compute_shader.get());
	}
};

}
