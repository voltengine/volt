#include <volt/pch.hpp>
#include <volt/video/d3d12/window.hpp>

namespace volt::video::d3d12::_internal {

using namespace math;

window::window(std::string &&title, uvec2 size) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	create(std::move(title), size);
}

}
