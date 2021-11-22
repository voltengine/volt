#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>

namespace volt::gpu {

class device;

class shader {
public:
	virtual ~shader() = default;

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<gpu::device> device;

	shader(std::shared_ptr<gpu::device> &&device)
			: device(std::move(device)) {}
};

}
