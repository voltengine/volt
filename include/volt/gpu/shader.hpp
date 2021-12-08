#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>

namespace volt::gpu {

class device;

class shader {
public:
	virtual ~shader() = default;

	const std::shared_ptr<gpu::device> &device() {
		return _device;
	}

protected:
	std::shared_ptr<gpu::device> _device;

	shader(std::shared_ptr<gpu::device> &&device)
			: _device(std::move(device)) {}
};

}
