#pragma once

#ifdef _WIN32
	#define VOLT_PLATFORM_WINDOWS
#elif __linux__
	#define VOLT_PLATFORM_LINUX
#else
	#error "Unsupported OS."
#endif

#ifdef VOLT_PLATFORM_WINDOWS
	#define VOLT_EXPORT __declspec(dllexport)
	#define VOLT_IMPORT __declspec(dllimport)
#elif defined(VOLT_PLATFORM_LINUX)
	#define VOLT_EXPORT __attribute__((visibility("default")))
	#define VOLT_IMPORT
#endif

#ifdef VOLT_BUILD
	#define VOLT_API VOLT_EXPORT
#else
	#define VOLT_API VOLT_IMPORT
#endif
