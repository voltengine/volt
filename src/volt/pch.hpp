#pragma once

#include <bitset>
#include <condition_variable>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <stack>
#include <typeindex>

#include <volt/macros.hpp>

#ifdef VOLT_PLATFORM_WINDOWS
	#include <Windows.h>
#endif
