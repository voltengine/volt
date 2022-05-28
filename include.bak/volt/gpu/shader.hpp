#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>

namespace volt::gpu {

class device;

class shader {
public:
	virtual ~shader() = default;

	VOLT_API const std::shared_ptr<gpu::device> &device() const;

protected:
	std::shared_ptr<gpu::device> _device;

	shader(std::shared_ptr<gpu::device> &&device);
};

}
