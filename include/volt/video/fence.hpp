#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>

namespace volt::video {

class device;

class fence {
public:
	virtual uint64_t value() = 0;

	virtual void signal(uint64_t value) = 0;

	virtual void wait(uint64_t value) = 0;

	const std::shared_ptr<video::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<video::device> device;

	fence(std::shared_ptr<video::device> &&device)
			: device(std::move(device)) {}
};

}
