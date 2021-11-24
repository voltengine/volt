#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"
#include "resource.hpp"

namespace volt::gpu {

class texture : public gpu::_resource, public std::enable_shared_from_this<texture> {
public:
	virtual ~texture() = default;

	texture_format get_format() {
		return format;
	}

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<gpu::device> device;
	texture_format format;

	texture(std::shared_ptr<gpu::device> &&device, texture_format format)
			: gpu::_resource(std::move(device)), format(format) {}
};

}
