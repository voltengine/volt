#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include "instance.hpp"

namespace volt::video {

class device;

class adapter : public std::enable_shared_from_this<adapter> {
public:
	virtual uint32_t vendor_id() = 0;

	virtual uint32_t device_id() = 0;

	virtual std::string name() = 0;

	virtual uint64_t dedicated_video_memory() = 0;

	virtual std::shared_ptr<video::device> create_device() = 0;

	const std::shared_ptr<video::instance> &get_instance() {
		return instance;
	}

protected:
	std::shared_ptr<video::instance> instance;

	adapter(std::shared_ptr<video::instance> &&instance)
			: instance(std::move(instance)) {}
};

}
