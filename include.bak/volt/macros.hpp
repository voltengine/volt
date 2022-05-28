#pragma once

#ifdef _WIN32
	#define VOLT_PLATFORM_WINDOWS
#elif __linux__
	#define VOLT_PLATFORM_LINUX
#else
	#error "Unsupported OS."
#endif

#ifdef VOLT_PLATFORM_LINUX
	#define _GNU_SOURCE

	#define GLFW_EXPOSE_NATIVE_X11
	#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif defined(VOLT_PLATFORM_WINDOWS)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

	#define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(VOLT_PLATFORM_MACOS)
	#define GLFW_EXPOSE_NATIVE_COCOA
#endif

#ifdef _MSC_VER
	#define VOLT_API_EXPORT __declspec(dllexport)
	#define VOLT_API_IMPORT __declspec(dllimport)
#else
	#define VOLT_API_EXPORT __attribute__((visibility("default")))
	#define VOLT_API_IMPORT
#endif

#ifdef VOLT_EXPORTS
	#define VOLT_API VOLT_API_EXPORT
#else
	#define VOLT_API VOLT_API_IMPORT
#endif

#define GLFW_INCLUDE_NONE
