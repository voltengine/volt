#include <volt/pch.hpp>
#include <volt/video/vk12/window.hpp>

#include <volt/error.hpp>

namespace volt::video::vk12::_internal {

using namespace math;

window::window(std::string &&title, uvec2 size, VkInstance vk_instance)
		: vk_instance(vk_instance) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	create(std::move(title), size);
	VOLT_ASSERT(glfwCreateWindowSurface(vk_instance, glfw_window, nullptr, &vk_surface) == VK_SUCCESS,
			"Failed to create window surface.")
}

window::~window() {
	vkDestroySurfaceKHR(vk_instance, vk_surface, nullptr);
}

}
