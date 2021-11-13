#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include "../math/math.hpp"
#include "monitor.hpp"

namespace volt::video {

class window {
public:
	VOLT_API virtual ~window();

	VOLT_API void process();

	VOLT_API bool is_closing();

	VOLT_API void set_closing(bool closing);

	VOLT_API std::string get_title();

	VOLT_API void set_title(std::string title);

	VOLT_API math::uvec2 get_pos();

	VOLT_API void set_pos(math::uvec2 pos);

	VOLT_API math::uvec2 get_size();

	VOLT_API void set_size(math::uvec2 size);

	VOLT_API bool is_maximized();

	VOLT_API void set_maximized(bool maximized);

	VOLT_API bool is_minimized();

	VOLT_API void set_minimized(bool minimized);

	VOLT_API bool is_fullscreen();

	VOLT_API void set_fullscreen(bool fullscreen);

	VOLT_API std::shared_ptr<monitor> get_monitor();

	VOLT_API void set_monitor(std::shared_ptr<monitor> monitor);

	VOLT_API bool is_visible();

	VOLT_API void set_visible(bool visible);

	VOLT_API bool focused();

	VOLT_API void focus();

	VOLT_API math::uvec2 get_frame_size();

	virtual std::shared_ptr<video::device> get_device() = 0;

	virtual void set_device(std::shared_ptr<video::device> device) = 0;

	virtual void present();

protected:
	GLFWwindow *glfw_window;

	VOLT_API window() = default;

	VOLT_API void create(std::string &&title, math::uvec2 size);

private:
	std::string title;
	math::uvec2 windowed_pos, windowed_size;
	// Maximized and minimized trackers are required, because
	// GLFW gives wrong answer once the window goes fullscreen 
	bool maximized = false, minimized = false, fullscreen = false;
	std::shared_ptr<monitor> monitor = video::monitor::primary();
	math::uvec2 frame_size;

	VOLT_API static void key_callback(GLFWwindow *glfw_window, int32_t key, int32_t scancode, int32_t action, int32_t mods);

	VOLT_API static void char_callback(GLFWwindow *glfw_window, uint32_t codepoint);

	VOLT_API static void mouse_button_callback(GLFWwindow *glfw_window, int32_t button, int32_t action, int32_t mods);

	VOLT_API static void scroll_callback(GLFWwindow *glfw_window, double x, double y);

	VOLT_API static void pos_callback(GLFWwindow *glfw_window, int32_t x, int32_t y);

	VOLT_API static void size_callback(GLFWwindow *glfw_window, int32_t width, int32_t height);

	VOLT_API static void iconify_callback(GLFWwindow *glfw_window, int32_t iconified);

	VOLT_API static void maximize_callback(GLFWwindow *glfw_window, int32_t maximized);

	VOLT_API static void framebuffer_size_callback(GLFWwindow *glfw_window, int32_t width, int32_t height);

	VOLT_API void apply_mode();
};

}