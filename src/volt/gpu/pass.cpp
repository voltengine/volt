#include <volt/pch.hpp>
#include <volt/gpu/pass.hpp>

namespace volt::gpu {

void _base_pass::constant_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer) {
	_constant_buffers[slot] = buffer.get();
}

void _base_pass::sampled_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture, const std::shared_ptr<gpu::sampler> &sampler) {
	_sampled_textures[slot] = { texture.get(), sampler.get() };
}

void _base_pass::storage_buffer(const std::string &slot, const std::shared_ptr<gpu::buffer> &buffer, bool shared) {
	_storage_buffers[slot] = { buffer.get(), shared };
}

void _base_pass::storage_texture(const std::string &slot, const std::shared_ptr<gpu::texture> &texture, bool shared) {
	_storage_textures[slot] = { texture.get(), shared };
}

void _base_pass::reset() {
	_constant_buffers.clear();
	_sampled_textures.clear();
	_storage_buffers.clear();
	_storage_textures.clear();
}

	VOLT_API const std::shared_ptr<gpu::device> &device();

protected:
	std::shared_ptr<gpu::device> _device;

	VOLT_API _base_pass(std::shared_ptr<gpu::device> &&device);

}
