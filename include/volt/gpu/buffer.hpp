#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::gpu {

class device;

class buffer {
public:
	virtual ~buffer() = default;

	virtual void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) = 0;

	virtual void unmap(size_t write_offset = 0, size_t write_size = 0) = 0;

	const std::shared_ptr<gpu::device> &device() {
		return _device;
	}

	size_t size() {
		return _size;
	}
	
protected:
	std::shared_ptr<gpu::device> _device;
	size_t _size;

	buffer(std::shared_ptr<gpu::device> &&device, size_t size)
			: _device(std::move(device)), _size(size) {}
};

}
