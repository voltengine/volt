#include <volt/pch.hpp>
#include <volt/gpu/texture.hpp>

namespace volt::gpu {

texture_features texture::features() const {
	return _features;
}

texture_format texture::format() const {
	return _format;
}

uint32_t texture::levels() const {
	return _levels;
}

math::uvec3 texture::size() const {
	return _size;
}

texture::texture(std::shared_ptr<gpu::device> &&device, texture_features features, texture_format format, uint32_t levels, math::uvec3 size)
		: gpu::resource(std::move(device)), _features(features), _format(format), _levels(levels), _size(size) {}

}
