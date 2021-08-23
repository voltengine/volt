#pragma once

#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <stack>
#include <typeindex>

#include <volt/macros.hpp>

#ifdef VOLT_PLATFORM_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX

	#include <Windows.h>
#endif
