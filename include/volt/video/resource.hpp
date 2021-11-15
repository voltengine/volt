#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::video {

class device;

class resource {
public:
	enum class type {
		internal,
		upload,
		write, // Requires resource::write(...) after access and before unmapping
		read   // Requires resource::read(...) after mapping and before access 
	};

	virtual void map(void **ptr) = 0;

	virtual void unmap() = 0;

	virtual void read(size_t offset, size_t size) = 0;

	virtual void write(size_t offset, size_t size) = 0;

	const std::shared_ptr<video::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<video::device> device;

	resource() = default;

	resource(std::shared_ptr<video::device> &&device)
			: device(std::move(device)) {}
};

}
