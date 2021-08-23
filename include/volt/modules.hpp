#pragma once

#include "macros.hpp"

#include <functional>
#include <string>

namespace volt::modules {

using load_callback = std::function<void()>;
using reload_callback = std::function<
		load_callback(const std::string &)>;

inline std::string _path_to_name(const std::string &path);

VOLT_API void load(const std::string &name);

VOLT_API void unload(const std::string &name);

VOLT_API void reload(const std::string &name);

VOLT_API void load_all();

VOLT_API void unload_all();

VOLT_API void reload_all();

VOLT_API void add_reload_callback(reload_callback &&callback);

inline std::string this_module();

VOLT_API void hello(); // TODO: Remove

}

#include "modules.inl"
