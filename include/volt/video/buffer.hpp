#pragma once

#include "../macros.hpp"

#include <memory>

#include "resource.hpp"

namespace volt::video {

class buffer : public video::_resource {
public:
	// virtual buffer_view create_view();

protected:
	buffer(std::shared_ptr<video::device> &&device)
			: video::_resource(std::move(device)) {}
};

}
