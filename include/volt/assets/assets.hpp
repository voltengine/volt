#pragma once

#include "../macros.hpp"

#include <concepts>
#include <functional>
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>

#include "../modules.hpp"

#define VOLT_ASSET

namespace volt::assets {

class asset;

template<typename T>
class asset_ref;

using asset_path = std::string;

VOLT_API const std::unordered_map<std::string, std::set<std::string>> &get_types();

#ifdef VOLT_DEVELOPMENT

VOLT_API std::set<asset_path> list_directories(const asset_path &path);

VOLT_API std::set<asset_path> list_assets(const asset_path &path);

// VOLT_API void create(const asset_path &path);

// VOLT_API void remove(const asset_path &path);

// VOLT_API void copy(const asset_path &src, const asset_path &dst);

// VOLT_API void move(const asset_path &src, const asset_path &dst);

// VOLT_API void import(const std::filesystem::path &src, const asset_path &dst_dir);

// VOLT_API void export_asset(const asset_path &src, const std::filesystem::path &dst);

#endif

VOLT_API asset_ref<asset> load(const asset_path &path);

// VOLT_API void reload(const asset_path &path);

// VOLT_API void reload();

VOLT_API const std::string &get_lang();

VOLT_API void set_lang(const std::string &lang);

}

namespace volt::assets::_internal {

class asset_owner;

VOLT_API extern std::unordered_map<std::string, std::set<std::string>> module_name_to_types;
VOLT_API extern std::unordered_map<std::type_index, std::string> type_index_to_type;
VOLT_API extern std::unordered_map<std::string, std::function<asset *()>> type_to_constructor;
VOLT_API extern std::unordered_map<std::string, asset_owner *> path_to_cached_owner;

#ifdef VOLT_DEVELOPMENT

VOLT_DEVELOPMENT_MODULE_LOAD_CALLBACK VOLT_API void development_module_load_callback();

VOLT_DEVELOPMENT_MODULE_UNLOAD_CALLBACK VOLT_API void development_module_unload_callback();

#endif

template<std::derived_from<asset> T>
void register_type(const std::string &type);

}

#include "assets.inl"

#include "asset.hpp"
#include "asset_ref.hpp"
