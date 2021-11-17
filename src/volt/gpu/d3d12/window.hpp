#pragma once

#include "../../macros.hpp"

#include "../window.hpp"

namespace volt::gpu::d3d12::_internal {

class window : public os::window {
public:
	window(std::string &&title, math::uvec2 size);
};

}
