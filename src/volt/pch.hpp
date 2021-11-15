#pragma once

#include <volt/macros.hpp>

#include <algorithm>
#include <bitset>
#include <chrono>
#include <codecvt>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <stack>
#include <typeindex>
#include <vector>

#ifdef VOLT_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <d3d12.h>
	#include <dxgi.h>
	#include <dxgi1_4.h>
#endif

#define GLFW_INCLUDE_NONE

#include <date/date.h>
#include <glad/vulkan.h>
#include <GLFW/glfw3.h>
#include <nlohmann/json.hpp>
#include <termcolor/termcolor.hpp>
#include <vk_mem_alloc.h>
