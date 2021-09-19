#pragma once

#include <volt/macros.hpp>

#include <bitset>
#include <chrono>
#include <condition_variable>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <stack>
#include <typeindex>
#include <vector>

#ifdef VOLT_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#include <date/date.h>
#include <nlohmann/json.hpp>
#include <termcolor/termcolor.hpp>
