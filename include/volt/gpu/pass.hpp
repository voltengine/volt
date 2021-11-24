#pragma once

#include "../macros.hpp"

#include <functional>
#include <map>
#include <memory>
#include <vector>

#include "../math/math.hpp"
#include "enums.hpp"
// #include "buffer.hpp"
// #include "queue.hpp"
#include "routine.hpp"
#include "shader.hpp"
#include "texture.hpp"

namespace volt::gpu {

class rasterization_pass {
public:
	// struct attachment {
	// 	texture_format format;
	// 	color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
	// 	load_op load_color_depth = clear | preserve | discard
	// 	store_op store_color_depth = store
	// 	load_op color_attachment.load_stencil = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	// 	store_op store_stencil = store;
	// 	reference_layout = initial_layout = VK_IMAGE_LAYOUT_UNDEFINED;
	// 	final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// };

	rasterization_pass &viewport(math::fvec2 width, math::
			fvec2 height, math::fvec2 depth = math::fvec2(0, 1));

	rasterization_pass &vertex_shader(std::shared_ptr<gpu::shader> &&shader);

	rasterization_pass &pixel_shader(std::shared_ptr<gpu::shader> &&shader);

	rasterization_pass &color_attachment(uint32_t location,
			std::shared_ptr<gpu::texture> &&texture,
			attachment_initializer initializer,
			math::fvec3 clear_color = math::fvec3::zero);

	rasterization_pass &depth_stencil_attachment(
			std::shared_ptr<gpu::texture> &&texture,
			attachment_initializer initializer,
			float clear_depth = 1,
			float clear_stencil = 0);

	// void set_sampler(uint32_t binding, std::shared_ptr<gpu::texture> &&texture, shader_stages stages);

	// void set_vertex_buffer(uint32_t location, std::shared_ptr<gpu::buffer> &&buffer);

	// void set_constant_buffer(uint32_t binding, std::shared_ptr<gpu::buffer> &&buffer, shader_stages stages);

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

// class compute_pass {
// public:
// 	void record_callback(std::function<void(compute_routine &)> &&callback);

// 	void compute_shader(const std::vector<uint8_t> &shader);

// 	void sampler(std::shared_ptr<gpu::texture> &&texture, uint32_t binding); // Accept samplers, input attachments (for TAA)

// 	void constant_buffer(std::shared_ptr<gpu::buffer> &&buffer, uint32_t binding); // Accept constant/storage buffers NO VERTEX
// };

// class copy_pass {
// public:
// 	void record_callback(std::function<void(copy_routine &)> &&callback);
// };

}
