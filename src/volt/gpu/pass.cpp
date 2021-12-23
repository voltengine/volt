#include <volt/pch.hpp>
#include <volt/gpu/pass.hpp>

namespace volt::gpu {

using namespace math;

rasterization_pass_info &rasterization_pass_info::viewport(
		fvec2 width, fvec2 height, fvec2 depth) {
	_viewport = { width, height, depth };
	return *this;
}

rasterization_pass_info &rasterization_pass_info::vertex_shader(std::shared_ptr<gpu::shader> shader) {
	_vertex_shader = std::move(shader);
	return *this;
}

rasterization_pass_info &rasterization_pass_info::pixel_shader(std::shared_ptr<gpu::shader> shader) {
	_pixel_shader = std::move(shader);
	return *this;
}

rasterization_pass_info &rasterization_pass_info::color_attachment(
		uint32_t location,
		std::shared_ptr<gpu::texture> texture,
		attachment_initializer initializer,
		math::fvec3 clear_color) {
	_color_attachment attachment{
		.texture = std::move(texture),
		.initializer = initializer,
		.clear_color = clear_color
	};
	_color_attachments[location] = attachment;
	return *this;
}

rasterization_pass_info &rasterization_pass_info::depth_stencil_attachment(
		std::shared_ptr<gpu::texture> texture,
		attachment_initializer initializer,
		float clear_depth,
		float clear_stencil) {
	class _depth_stencil_attachment attachment{
		.texture = std::move(texture),
		.initializer = initializer,
		.clear_depth = clear_depth,
		.clear_stencil = clear_stencil
	};
	this->_depth_stencil_attachment = attachment;
	return *this;
}

compute_pass_info &compute_pass_info::compute_shader(std::shared_ptr<gpu::shader> shader) {
	_compute_shader = std::move(shader);
	return *this;
}

void _base_pass::constant_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer) {
	constant_buffers[slot] = buffer.get();
}

void _base_pass::sampled_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture) {
	sampled_textures[slot] = texture.get();
}

void _base_pass::storage_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer, bool shared) {
	storage_buffers[slot] = std::make_pair(buffer.get(), shared);
}

void _base_pass::storage_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture, bool shared) {
	storage_textures[slot] = std::make_pair(texture.get(), shared);
}

void _base_pass::reset() {
	constant_buffers.clear();
	sampled_textures.clear();
	storage_buffers.clear();
	storage_textures.clear();
}

}
