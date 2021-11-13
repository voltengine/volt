#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include "device.hpp"

namespace volt::video {

class adapter {
public:
	virtual std::shared_ptr<video::device> create_device() = 0;

	virtual uint32_t vendor_id() = 0;

	virtual uint32_t device_id() = 0;

	virtual std::string name() = 0;

	virtual uint64_t dedicated_video_memory() = 0;

protected:
	VOLT_API adapter() = default;
};

}
