#include <volt/pch.hpp>
#include <volt/os/window.hpp>

#include <volt/math/math.hpp>
#include <volt/error.hpp>

namespace volt::os {

using namespace math;

window::window(std::string &&title, uvec2 size) {
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	_glfw_window = glfwCreateWindow(size.x, size.y, title.c_str(), nullptr, nullptr);
	VOLT_ASSERT(_glfw_window, "Failed to create window.")

	glfwSetWindowUserPointer(_glfw_window, this);

	glfwSetKeyCallback(_glfw_window, key_callback);
	glfwSetCharCallback(_glfw_window, char_callback);
	glfwSetMouseButtonCallback(_glfw_window, mouse_button_callback);
	glfwSetScrollCallback(_glfw_window, scroll_callback);
	glfwSetWindowPosCallback(_glfw_window, pos_callback);
	glfwSetWindowSizeCallback(_glfw_window, size_callback);
	glfwSetWindowIconifyCallback(_glfw_window, iconify_callback);
	glfwSetWindowMaximizeCallback(_glfw_window, maximize_callback);
	glfwSetFramebufferSizeCallback(_glfw_window, framebuffer_size_callback);

	this->title = title;

	ivec2 glfw_pos, glfw_size, glfw_frame_size;
	glfwGetWindowPos(_glfw_window, &glfw_pos.x, &glfw_pos.y);
	glfwGetWindowSize(_glfw_window, &glfw_size.x, &glfw_size.y);
	glfwGetFramebufferSize(_glfw_window, &glfw_frame_size.x, &glfw_frame_size.y);
	windowed_pos = glfw_pos;
	windowed_size = glfw_size;
	frame_size = glfw_frame_size;

	if (monitor != nullptr)
		set_pos((monitor->resolution() - size) / 2);	
}

window::~window() {
	glfwDestroyWindow(_glfw_window);
}

void window::process() {
	int32_t cursor_mode = glfwGetInputMode(_glfw_window, GLFW_CURSOR);
	//if (Engine.getInstance().getInputManager().isMouseHidden()) {
	//	if (cursorMode != GLFW_CURSOR_HIDDEN)
	//		glfwSetInputMode(_glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	//} else if (cursorMode != GLFW_CURSOR_NORMAL)
	//	glfwSetInputMode(_glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	//uvec2 new_pos = glfwGetCursorPos(glfw_window, posX, posY);
	//uvec2 delta_pos = new_pos - last_cursor_pos;

	//InputManager inputManager = Engine.getInstance().getInputManager();
	//if (inputManager.isMouseCentered()) {
	//	lastCursorPos = glfwGetWindowSize(_glfw_window, sizeX, sizeY).div(2);
	//	glfwSetCursorPos(_glfw_window, lastCursorPos.getX(), lastCursorPos.getY());
	//} else lastCursorPos = newPos;

	//inputManager.setAxisValue(InputAxis.MOUSE_X, deltaPos.getX());
	//inputManager.setAxisValue(InputAxis.MOUSE_Y, deltaPos.getY());
}

bool window::is_closing() {
	return glfwWindowShouldClose(_glfw_window);
}

void window::set_closing(bool closing) {
	glfwSetWindowShouldClose(_glfw_window, closing);
}

std::string window::get_title() {
	return title;
}

void window::set_title(std::string title) {
	glfwSetWindowTitle(_glfw_window, title.c_str());
	this->title = std::move(title);
}

uvec2 window::get_pos() {
	return windowed_pos;
}

void window::set_pos(uvec2 pos) const {
	if (!fullscreen)
		glfwSetWindowPos(_glfw_window, pos.x, pos.y);
}

uvec2 window::get_size() {
	return windowed_size;
}

void window::set_size(uvec2 size) const {
	if (!fullscreen)
		glfwSetWindowSize(_glfw_window, size.x, size.y);
}

bool window::is_maximized() const {
	return maximized;
}

void window::set_maximized(bool maximized) {
	if (fullscreen) return;
	this->maximized = maximized;
	apply_mode();
}

bool window::is_minimized() const {
	return minimized;
}

void window::set_minimized(bool minimized) {
	if (fullscreen) return;
	this->minimized = minimized;
	apply_mode();
}

bool window::is_fullscreen() const {
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
		glfwSetWindowMonitor(_glfw_window, monitor->_glfw_monitor, 0, 0,
				size.x, size.y, GLFW_DONT_CARE);
	} else {
		glfwSetWindowMonitor(_glfw_window, nullptr, windowed_pos.x, windowed_pos.y,
				windowed_size.x, windowed_size.y, GLFW_DONT_CARE);
		apply_mode();
	}
}

std::shared_ptr<monitor> window::get_monitor() {
	return monitor;
}

void window::set_monitor(std::shared_ptr<os::monitor> monitor) {
	this->monitor = std::move(monitor);

	if (fullscreen) {
		set_fullscreen(false);
		set_fullscreen(true);
	}
}

bool window::is_visible() const {
	return glfwGetWindowAttrib(_glfw_window, GLFW_VISIBLE) == GLFW_TRUE;
}

void window::set_visible(bool visible) {
	if (visible)
		glfwShowWindow(_glfw_window);
	else
		glfwHideWindow(_glfw_window);
}

bool window::focused() const {
	return glfwGetWindowAttrib(_glfw_window, GLFW_FOCUSED) == GLFW_TRUE;
}

void window::focus() {
	glfwFocusWindow(_glfw_window);
}

uvec2 window::get_frame_size() {
	return frame_size;
}

bool window::has_surface() const {
	return surface_constructed;
}

void window::_construct_swapchain() {
	VOLT_ASSERT(!surface_constructed, "Window already has a swapchain. It must be destroyed before creating a new one.");
	surface_constructed = true;
}

void window::_destruct_swapchain() {
	surface_constructed = false;
}

void window::_on_frame_resize(std::function<void()> &&callback) {
	_frame_resize_callback = std::move(callback);
}

void window::key_callback(GLFWwindow *glfw_window, int32_t key, int32_t scancode, int32_t action, int32_t mods) {}

void window::char_callback(GLFWwindow *glfw_window, uint32_t codepoint) {}

void window::mouse_button_callback(GLFWwindow *glfw_window, int32_t button, int32_t action, int32_t mods) {}

void window::scroll_callback(GLFWwindow *glfw_window, double x, double y) {}

void window::pos_callback(GLFWwindow *glfw_window, int32_t x, int32_t y) {
	window *window = static_cast<os::window *>(glfwGetWindowUserPointer(glfw_window));

	// We can't rely on the tracking variables as callbacks may fire in the wrong order
	bool maximized = (glfwGetWindowAttrib(glfw_window, GLFW_MAXIMIZED) == GLFW_TRUE);
	bool minimized = (glfwGetWindowAttrib(glfw_window, GLFW_ICONIFIED) == GLFW_TRUE);

	if (window->fullscreen || maximized || minimized)
		return;
	
	window->windowed_pos.x = x;
	window->windowed_pos.y = y;
}

void window::size_callback(GLFWwindow *glfw_window, int32_t width, int32_t height) {
	window *window = static_cast<os::window *>(glfwGetWindowUserPointer(glfw_window));

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
	auto window = static_cast<os::window *>(glfwGetWindowUserPointer(glfw_window));

	window->frame_size.x = math::max(width, 1);
	window->frame_size.y = math::max(height, 1);

	if (window->_frame_resize_callback)
		window->_frame_resize_callback();
}

void window::apply_mode() {
	// Minimization overrides maximization, they both override windowed mode:
	if (maximized)
		glfwMaximizeWindow(_glfw_window);
	if (minimized)
		glfwIconifyWindow(_glfw_window);
	if (!maximized && !minimized)
		glfwRestoreWindow(_glfw_window);
}

}
