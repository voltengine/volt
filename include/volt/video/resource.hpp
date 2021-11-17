#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::video {

class device;

class _resource {
public:
	virtual void map(void **ptr) = 0;

	virtual void unmap() = 0;

	virtual void read(size_t offset, size_t size) = 0;

	virtual void write(size_t offset, size_t size) = 0;

	const std::shared_ptr<video::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<video::device> device;

	_resource() = default;

	_resource(std::shared_ptr<video::device> &&device)
			: device(std::move(device)) {}
};

}
