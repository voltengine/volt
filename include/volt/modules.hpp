#pragma once

#include "macros.hpp"

#include <functional>
#include <set>
#include <string>
#include <unordered_map>

#define VOLT_DEVELOPMENT_MODULE_LOAD_CALLBACK
#define VOLT_DEVELOPMENT_MODULE_UNLOAD_CALLBACK

namespace volt::modules {

inline std::string this_module_name();

#ifdef VOLT_DEVELOPMENT

VOLT_API void reload_development_module();

#endif

VOLT_API const std::set<std::string> &names();

}

namespace volt::modules::_internal {

VOLT_API extern std::unordered_map<std::string, std::vector<
		std::function<void()>>> module_name_to_development_module_unload_callbacks;
VOLT_API extern std::unordered_map<std::string, std::vector<
		std::function<void()>>> module_name_to_development_module_load_callbacks;

VOLT_API std::string path_to_name(const std::string &path);

VOLT_API void load();

VOLT_API void unload();

#ifdef VOLT_DEVELOPMENT

// Load callbacks are executed after the module is loaded and after volt_module_main() was called
inline void register_development_module_load_callback(const std::function<void()> &callback);

// Load callbacks are executed before any unloading happens
inline void register_development_module_unload_callback(const std::function<void()> &callback);

#endif

}

#include "modules.inl"
