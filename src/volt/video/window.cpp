#include <volt/pch.hpp>
#include <volt/video/window.hpp>

#include <volt/math/math.hpp>
#include <volt/error.hpp>

namespace volt::video {

using namespace math;

window::~window() {
	glfwDestroyWindow(glfw_window);
}

void window::process() {
	int32_t cursor_mode = glfwGetInputMode(glfw_window, GLFW_CURSOR);
	//if (Engine.getInstance().getInputManager().isMouseHidden()) {
	//	if (cursorMode != GLFW_CURSOR_HIDDEN)
	//		glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	//} else if (cursorMode != GLFW_CURSOR_NORMAL)
	//	glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//uvec2 new_pos = glfwGetCursorPos(glfw_window, posX, posY);
	//uvec2 delta_pos = new_pos - last_cursor_pos;

	//InputManager inputManager = Engine.getInstance().getInputManager();
	//if (inputManager.isMouseCentered()) {
	//	lastCursorPos = glfwGetWindowSize(glfw_window, sizeX, sizeY).div(2);
	//	glfwSetCursorPos(glfw_window, lastCursorPos.getX(), lastCursorPos.getY());
	//} else lastCursorPos = newPos;

	//inputManager.setAxisValue(InputAxis.MOUSE_X, deltaPos.getX());
	//inputManager.setAxisValue(InputAxis.MOUSE_Y, deltaPos.getY());
}

bool window::is_closing() {
	return glfwWindowShouldClose(glfw_window);
}

void window::set_closing(bool closing) {
	glfwSetWindowShouldClose(glfw_window, closing);
}

std::string window::get_title() {
	return title;
}

void window::set_title(std::string title) {
	glfwSetWindowTitle(glfw_window, title.c_str());
	this->title = std::move(title);
}

uvec2 window::get_pos() {
	return windowed_pos;
}

void window::set_pos(uvec2 pos) {
	if (!fullscreen)
		glfwSetWindowPos(glfw_window, pos.x, pos.y);
}

uvec2 window::get_size() {
	return windowed_size;
}

void window::set_size(uvec2 size) {
	if (!fullscreen)
		glfwSetWindowSize(glfw_window, size.x, size.y);
}

bool window::is_maximized() {
	return maximized;
}

void window::set_maximized(bool maximized) {
	if (fullscreen) return;
	this->maximized = maximized;
	apply_mode();
}

bool window::is_minimized() {
	return minimized;
}

void window::set_minimized(bool minimized) {
	if (fullscreen) return;
	this->minimized = minimized;
	apply_mode();
}

bool window::is_fullscreen() {
	return fullscreen;
}

void window::set_fullscreen(bool fullscreen) {
	if (this->fullscreen == fullscreen || monitor == nullptr)
		return;
	this->fullscreen = fullscreen;

	if (fullscreen) {
		set_visible(true);
		maximized = minimized = false;
		apply_mode();

		uvec2 size = monitor->resolution();
		glfwSetWindowMonitor(glfw_window, monitor->_glfw_monitor, 0, 0,
				size.x, size.y, GLFW_DONT_CARE);
	} else {
		glfwSetWindowMonitor(glfw_window, nullptr, windowed_pos.x, windowed_pos.y,
				windowed_size.x, windowed_size.y, GLFW_DONT_CARE);
		apply_mode();
	}
}

std::shared_ptr<monitor> window::get_monitor() {
	return monitor;
}

void window::set_monitor(std::shared_ptr<video::monitor> monitor) {
	this->monitor = std::move(monitor);

	if (fullscreen) {
		set_fullscreen(false);
		set_fullscreen(true);
	}
}

bool window::is_visible() {
	return glfwGetWindowAttrib(glfw_window, GLFW_VISIBLE) == GLFW_TRUE;
}

void window::set_visible(bool visible) {
	if (visible)
		glfwShowWindow(glfw_window);
	else
		glfwHideWindow(glfw_window);
}

bool window::focused() {
	return glfwGetWindowAttrib(glfw_window, GLFW_FOCUSED) == GLFW_TRUE;
}

void window::focus() {
	glfwFocusWindow(glfw_window);
}

uvec2 window::get_frame_size() {
	return frame_size;
}

void window::create(std::string &&title, uvec2 size) {
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	glfw_window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
	VOLT_ASSERT(glfw_window, "Failed to create window.")

	glfwSetWindowUserPointer(glfw_window, this);

	glfwSetKeyCallback(glfw_window, key_callback);
	glfwSetCharCallback(glfw_window, char_callback);
	glfwSetMouseButtonCallback(glfw_window, mouse_button_callback);
	glfwSetScrollCallback(glfw_window, scroll_callback);
	glfwSetWindowPosCallback(glfw_window, pos_callback);
	glfwSetWindowSizeCallback(glfw_window, size_callback);
	glfwSetWindowIconifyCallback(glfw_window, iconify_callback);
	glfwSetWindowMaximizeCallback(glfw_window, maximize_callback);
	glfwSetFramebufferSizeCallback(glfw_window, framebuffer_size_callback);

	this->title = title;

	ivec2 glfw_pos, glfw_size, glfw_frame_size;
	glfwGetWindowPos(glfw_window, &glfw_pos.x, &glfw_pos.y);
	glfwGetWindowSize(glfw_window, &glfw_size.x, &glfw_size.y);
	glfwGetFramebufferSize(glfw_window, &glfw_frame_size.x, &glfw_frame_size.y);
	windowed_pos = glfw_pos;
	windowed_size = glfw_size;
	frame_size = glfw_frame_size;

	if (monitor != nullptr)
		set_pos((monitor->resolution() - size) / 2);	
}

void window::key_callback(GLFWwindow *glfw_window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {}

void window::char_callback(GLFWwindow *glfw_window, uint32_t codepoint) {}

void window::mouse_button_callback(GLFWwindow *glfw_window, int32_t button, int32_t action, int32_t mods) {}

void window::scroll_callback(GLFWwindow *glfw_window, double x, double y) {}

void window::pos_callback(GLFWwindow *glfw_window, int32_t x, int32_t y) {
	window *window = static_cast<video::window *>(glfwGetWindowUserPointer(glfw_window));

	// We can't rely on the tracking variables as callbacks may fire in the wrong order
	bool maximized = (glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED) == GLFW_TRUE);
	bool minimized = (glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED) == GLFW_TRUE);

	if (window->fullscreen || maximized || minimized)
		return;
	
	window->windowed_pos.x = x;
	window->windowed_pos.y = y;
}

void window::size_callback(GLFWwindow *glfw_window, int32_t width, int32_t height) {
	window *window = static_cast<video::window *>(glfwGetWindowUserPointer(glfw_window));

	// We can't rely on the tracking variables as callbacks may fire in the wrong order
	bool maximized = (glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED) == GLFW_TRUE);
	bool minimized = (glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED) == GLFW_TRUE);

	if (window->fullscreen || maximized || minimized)
		return;
	
	window->windowed_size.x = math::max(width, 1);
	window->windowed_size.y = math::max(height, 1);
}

void window::iconify_callback(GLFWwindow *glfw_window, int32_t iconified) {
	static_cast<window *>(glfwGetWindowUserPointer(
			glfw_window))->minimized = iconified;
}

void window::maximize_callback(GLFWwindow *glfw_window, int32_t maximized) {
	static_cast<window *>(glfwGetWindowUserPointer(
			glfw_window))->maximized = maximized;
}

void window::framebuffer_size_callback(GLFWwindow *glfw_window, int32_t width, int32_t height) {
	window *window = static_cast<video::window *>(
			glfwGetWindowUserPointer(glfw_window));

	window->frame_size.x = math::max(width, 1);
	window->frame_size.y = math::max(height, 1);
}

void window::apply_mode() {
	// Minimization overrides maximization, they both override windowed mode:
	if (maximized)
		glfwMaximizeWindow(glfw_window);
	if (minimized)
		glfwIconifyWindow(glfw_window);
	if (!maximized && !minimized)
		glfwRestoreWindow(glfw_window);
}

}
