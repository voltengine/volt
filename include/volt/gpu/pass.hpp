#pragma once

#include "../macros.hpp"

#include <functional>
#include <map>
#include <memory>
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

private:
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
	std::map<uint32_t, _color_attachment> color_attachments;
	_depth_stencil_attachment _depth_stencil_attachment;
};

class compute_pass_info {
public:
	VOLT_API compute_pass_info &compute_shader(std::shared_ptr<gpu::shader> shader);

private:
	std::shared_ptr<gpu::shader> _compute_shader;
};

#pragma endregion

class ray_tracing_pass;

class rasterization_pass {
public:
	virtual void index_buffer(uint32_t location, std::shared_ptr<gpu::buffer> &&buffer) = 0;

	virtual void vertex_buffer(uint32_t location, std::shared_ptr<gpu::buffer> &&buffer) = 0;

	virtual void sampler(std::string_view slot, std::shared_ptr<gpu::texture> &&texture) = 0;

	virtual void constant_buffer(std::string_view slot, std::shared_ptr<gpu::buffer> &&buffer) = 0;

	virtual void draw(uint32_t index_count, uint32_t instance_count = 1) = 0;
};

class compute_pass {
public:
	virtual void sampler(std::string_view slot, std::shared_ptr<gpu::texture> &&texture) = 0;

	virtual void constant_buffer(std::string_view slot, std::shared_ptr<gpu::buffer> &&buffer) = 0;

	virtual void dispatch(math::uvec3 group_count) = 0;
};

}
