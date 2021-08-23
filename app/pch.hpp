#pragma once

#include <chrono>
#include <filesystem>
#include <iostream>
#include <string>
#include <thread>

#include <volt/volt.hpp>

#ifdef VOLT_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

	#include <Windows.h>
#endif
