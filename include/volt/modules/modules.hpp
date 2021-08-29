#pragma once

#include "../macros.hpp"

#include <functional>
#include <map>
#include <set>
#include <string>

#include "instance.hpp"

namespace volt::modules {

VOLT_API extern std::map<std::string, std::set<std::string>
		> _module_name_to_serializable_names;
VOLT_API extern std::map<std::string, std::function<
		serializable *()>> _serializable_name_to_constructor;
VOLT_API extern std::map<std::string, std::list<_instance_owner *
		>> _serializable_name_to_instance_tracking_list;

inline std::string _path_to_name(const std::string &path);

VOLT_API void load(const std::string &name);

VOLT_API void unload(const std::string &name);

VOLT_API void reload(const std::vector<std::string> &names, const std::function<void()> &callback);

VOLT_API void load();

VOLT_API void unload();

VOLT_API void reload(const std::function<void()> &callback);

inline std::string this_module();

template<typename T>
void register_serializable(const std::string &name);

VOLT_API shared_instance<> instantiate_serializable(const std::string &name);

}

#include "modules.inl"
