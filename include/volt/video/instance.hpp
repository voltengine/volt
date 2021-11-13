#pragma once

#include "../macros.hpp"

#include <memory>
#include <string>
#include <vector>

#include "../math/math.hpp"
#include "adapter.hpp"
#include "window.hpp"

namespace volt::video {

class instance {
public:
	virtual std::vector<std::shared_ptr<video::adapter>> list_adapters() = 0;

	virtual std::shared_ptr<video::window> create_window(std::string title, math::uvec2 size) = 0;

protected:
	VOLT_API instance() = default;
};

}
