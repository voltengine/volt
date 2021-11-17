#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"
#include "resource.hpp"
#include "texture_view.hpp"

namespace volt::video {

class texture : public video::_resource, public std::enable_shared_from_this<texture> {
public:
	virtual std::shared_ptr<video::texture_view> create_view(
			video::texture_view_type type,
			video::texture_view_aspects aspects) = 0;

protected:
	texture(std::shared_ptr<video::device> &&device)
			: video::_resource(std::move(device)) {}
};

}
