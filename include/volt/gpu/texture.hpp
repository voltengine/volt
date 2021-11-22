#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"
#include "resource.hpp"
#include "texture_view.hpp"

namespace volt::gpu {

class texture : public gpu::_resource, public std::enable_shared_from_this<texture> {
public:
	virtual ~texture() = default;

	virtual std::shared_ptr<gpu::texture_view> create_view(
			gpu::texture_view_type type,
			gpu::texture_view_aspects aspects) = 0;

	texture_format get_format() {
		return format;
	}

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

protected:
	std::shared_ptr<gpu::device> device;
	texture_format format;
	texture_features features;

	texture(std::shared_ptr<gpu::device> &&device, texture_format format, texture_features features)
			: gpu::_resource(std::move(device)), format(format), features(features) {}
};

}
