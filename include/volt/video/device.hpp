#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "shader.hpp"
#include "swapchain.hpp"
#include "pipeline.hpp"
#include "window.hpp"

namespace volt::video {

class device {
public:
	virtual std::shared_ptr<video::swapchain> create_swapchain(const std::shared_ptr<video::window> &window) = 0;

	virtual std::shared_ptr<video::shader> create_shader(const std::vector<uint8_t> &bytecode) = 0;
	
	virtual std::shared_ptr<video::pipeline> create_pipeline() = 0;

protected:
	VOLT_API device() = default;
};

}
