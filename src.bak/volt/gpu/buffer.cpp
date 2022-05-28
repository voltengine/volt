#include <volt/pch.hpp>
#include <volt/gpu/buffer.hpp>

namespace volt::gpu {

buffer_features buffer::features() const {
	return _features;
}

size_t buffer::size() const {
	return _size;
}

buffer::buffer(std::shared_ptr<gpu::device> &&device, buffer_features features, size_t size)
		: gpu::resource(std::move(device)), _features(features), _size(size) {}

}
