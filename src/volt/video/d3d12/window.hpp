#pragma once

#include "../../macros.hpp"

#include "../window.hpp"

namespace volt::video::d3d12::_internal {

class window : public os::window {
public:
	VOLT_API window(std::string &&title, math::uvec2 size);
};

}
