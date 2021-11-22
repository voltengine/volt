#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::gpu {

class device;

class _resource {
public:
	virtual ~_resource() = default;

	virtual void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) = 0;

	virtual void unmap(size_t write_offset = 0, size_t write_size = 0) = 0;

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<gpu::device> device;

	_resource() = default;

	_resource(std::shared_ptr<gpu::device> &&device)
			: device(std::move(device)) {}
};

}
