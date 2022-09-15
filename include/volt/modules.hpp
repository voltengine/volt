#pragma once

#include "macros.hpp"

#include <functional>
#include <set>
#include <string>
#include <unordered_map>

#define VOLT_DEVELOPMENT_MODULE_LOAD_CALLBACK
#define VOLT_DEVELOPMENT_MODULE_UNLOAD_CALLBACK
#define VOLT_UNLOAD_ALL_MODULES_CALLBACK

namespace volt::modules {

#ifdef VOLT_DEVELOPMENT

VOLT_API void reload_development_module();

#endif

VOLT_API const std::set<std::string> &names();

}

namespace volt::modules::_internal {

VOLT_API std::string path_to_name(const std::string &path);

VOLT_API void load();

VOLT_API void unload();

#ifdef VOLT_DEVELOPMENT

// Executed after the module is loaded and after volt_module_main() was called; Used only to recover state during development module reload
VOLT_API void register_development_module_load_callback(const std::string &module_name, std::function<void()> callback);

// Executed before any unloading happens; Used before development module reload or when initial loading fails or when loading fails during reload
VOLT_API void register_development_module_unload_callback(const std::string &module_name, std::function<void()> callback);

#endif

// Executed before any unloading happens
VOLT_API void register_unload_all_modules_callback(const std::string &module_name, std::function<void()> callback);

}
