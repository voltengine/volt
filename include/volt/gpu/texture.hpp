#pragma once

#include "../macros.hpp"

#include <memory>

#include "../math/math.hpp"
#include "enums.hpp"

namespace volt::gpu {

class device;

class texture : public std::enable_shared_from_this<texture> {
public:
	virtual ~texture() = default;

	virtual void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) = 0;

	virtual void unmap(size_t write_offset = 0, size_t write_size = 0) = 0;

	const std::shared_ptr<gpu::device> &device() {
		return _device;
	}

	texture_format format() {
		return _format;
	}

	uint32_t levels() {
		return _levels;
	}

	math::uvec3 size() {
		return _size;
	}

protected:
	std::shared_ptr<gpu::device> _device;
	texture_format _format;
	uint32_t _levels;
	math::uvec3 _size;

	texture(std::shared_ptr<gpu::device> &&device, texture_format format, uint32_t levels, math::uvec3 size)
			: _device(std::move(device)), _format(format), _levels(levels), _size(size) {}
};

}
