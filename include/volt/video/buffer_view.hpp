#pragma once

#include "../macros.hpp"

#include <memory>

#include "buffer.hpp"

namespace volt::video {

class buffer_view {
protected:
	std::shared_ptr<video::buffer> buffer;

	VOLT_API buffer_view(std::shared_ptr<video::buffer> &&buffer);
};

}
