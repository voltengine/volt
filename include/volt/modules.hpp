#pragma once

#include "macros.hpp"

#include <functional>
#include <map>
#include <set>
#include <string>

namespace volt::modules {

using unload_callback = std::function<void(const std::string &, bool)>;

#ifdef VOLT_PLATFORM_LINUX
constexpr const char *module_extension = ".so";
#elif defined(VOLT_PLATFORM_WINDOWS)
constexpr const char *module_extension = ".dll";
#endif

VOLT_API extern std::map<std::string, std::vector<
		unload_callback>> _module_name_to_unload_callbacks;

inline std::string _path_to_name(const std::string &path);

inline std::string this_module_name();

VOLT_API void load();

VOLT_API void unload();

#ifdef VOLT_DEVELOPMENT
VOLT_API void reload_development_module(
		const std::function<void()> &callback);
#endif

VOLT_API const std::set<std::string> &get_names();

inline void register_unload_callback(const unload_callback &callback);

}

#include "modules.inl"
