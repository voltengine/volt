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
#include <thread>
#include <tuple>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifdef VOLT_PLATFORM_WINDOWS
	#include <Windows.h>
	#include <d3d12.h>
	#include <D3D12MemAlloc.h>
	#include <d3d12sdklayers.h>
	#include <dxgi.h>
	#include <dxgi1_4.h>
#endif

#include <date/date.h>
// #include <glad/gl.h>
#include <glad/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <nlohmann/json.hpp>
#include <spirv_reflect.h>
#include <termcolor/termcolor.hpp>
#include <vk_mem_alloc.h>
