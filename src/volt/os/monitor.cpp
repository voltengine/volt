#include <volt/pch.hpp>
#include <volt/os/monitor.hpp>

#include <volt/error.hpp>

namespace volt::os {

using namespace math;

std::shared_ptr<monitor> monitor::primary() {
	GLFWmonitor *glfw_monitor = glfwGetPrimaryMonitor();

	if (glfw_monitor == nullptr)
		return nullptr;
	else
		return std::shared_ptr<monitor>(new monitor(glfw_monitor));
}

std::vector<std::shared_ptr<monitor>> monitor::enumerate() {
	int32_t num_glfw_monitors;
	GLFWmonitor **glfw_monitors = glfwGetMonitors(&num_glfw_monitors);
	VOLT_ASSERT(glfw_monitors, "Failed to enumerate monitors.")

	std::vector<std::shared_ptr<monitor>> monitors(num_glfw_monitors);
	std::transform(
		glfw_monitors,
		glfw_monitors + num_glfw_monitors,
		monitors.begin(),
		[](GLFWmonitor *glfw_monitor) {
			return std::shared_ptr<monitor>(new monitor(glfw_monitor));
		}
	);
	return monitors;
}

uvec2 monitor::resolution() {
	return uvec2(glfw_video_mode->width, glfw_video_mode->height);
}

uvec3 monitor::color_depth() {
	return uvec3(
		glfw_video_mode->redBits,
		glfw_video_mode->greenBits,
		glfw_video_mode->blueBits
	);
}

uint32_t monitor::refresh_rate() {
	return glfw_video_mode->refreshRate;
}

monitor::monitor(GLFWmonitor *glfw_monitor) : _glfw_monitor(glfw_monitor) {
	VOLT_ASSERT(glfw_monitor,
			"GLFW monitor must not be NULL.")
	VOLT_ASSERT(glfw_video_mode = glfwGetVideoMode(glfw_monitor),
			"Failed to acquire video glfw_video_mode of a monitor.")
}

}