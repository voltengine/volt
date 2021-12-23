#pragma once

#include "macros.hpp"

#include <functional>
#include <set>
#include <string>
#include <unordered_map>

#define VOLT_MODULE_LOAD_CALLBACK
#define VOLT_MODULE_UNLOAD_CALLBACK

namespace volt::modules {

using load_callback = std::function<void(const std::string &)>;
using unload_callback = load_callback;

#ifdef VOLT_PLATFORM_LINUX
constexpr char module_extension[] = ".so";
#elif defined(VOLT_PLATFORM_WINDOWS)
constexpr char module_extension[] = ".dll";
#endif

inline std::string this_module_name();

VOLT_API void load();

VOLT_API void unload();

#ifdef VOLT_DEVELOPMENT

VOLT_API void reload_development_module();

#endif

VOLT_API const std::set<std::string> &get_names();

inline void register_load_callback(const load_callback &callback);

inline void register_unload_callback(const unload_callback &callback);

}

namespace volt::modules::_internal {

VOLT_API extern std::unordered_map<std::string, std::vector<
		unload_callback>> module_name_to_unload_callbacks;
VOLT_API extern std::unordered_map<std::string, std::vector<
		load_callback>> module_name_to_load_callbacks;

inline std::string path_to_name(const std::string &path);

}

#include "modules.inl"
