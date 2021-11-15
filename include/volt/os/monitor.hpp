#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include <GLFW/glfw3.h>

#include "../math/math.hpp"

namespace volt::os {

class monitor {
public:
	GLFWmonitor *_glfw_monitor;

	VOLT_API static std::shared_ptr<monitor> primary();

	VOLT_API static std::vector<std::shared_ptr<monitor>> enumerate();

	VOLT_API math::uvec2 resolution();

	VOLT_API math::uvec3 color_depth();

	VOLT_API uint32_t refresh_rate();

private:
	VOLT_API monitor(GLFWmonitor *handle);

	const GLFWvidmode *glfw_video_mode;
};

}
