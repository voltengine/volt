#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"

namespace volt::gpu {

class device;

class pipeline {
public:
	void add_shader(shader);

	void add_binding();

	void build();

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<gpu::device> device;

	pipeline(std::shared_ptr<gpu::device> &&device)
			: device(std::move(device)) {}
};

}
