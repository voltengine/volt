#include <volt/pch.hpp>
#include <volt/assets/assets.hpp>

#include <volt/assets/asset.hpp>
#include <volt/assets/asset_ref.hpp>
#include <volt/util/util.hpp>
#include <volt/error.hpp>

namespace fs = std::filesystem;
namespace nl = nlohmann;

#ifdef VOLT_DEVELOPMENT
static std::map<std::string, std::map<std::string, nlohmann::json>> development_reload_snapshot;
#endif

static std::string lang;

namespace volt::assets {

using namespace _internal;

#ifdef VOLT_DEVELOPMENT

static void iterate_development_paths(const std::function<bool(fs::path &)> &callback) {
	std::vector<std::string> paths = util::split(util::read_text_file(
			fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "paths.txt"), "\n");

	auto path = fs::path(VOLT_DEVELOPMENT_PATH) / "assets";
	if (fs::exists(path) && callback(path))
		return;

	for (auto it = paths.rbegin(); it != paths.rend(); it++) {
		path = fs::path(it->substr(it->find(' ') + 1)) / "assets";
		if (fs::exists(path) && callback(path))
			break; 
	}
}

#endif

static asset *load_asset() {

}

const std::map<std::string, std::set<std::string>> &get_types() {
	return module_name_to_types;
}

#ifdef VOLT_DEVELOPMENT

std::set<asset_path> list_directories(const asset_path &path) {
	std::set<asset_path> directories;

	iterate_development_paths([&](fs::path &root_path) {
		root_path /= path;
		for (auto &item : fs::directory_iterator(root_path)) {
			if (item.is_directory())
				directories.emplace(std::filesystem::relative(item.path(), root_path).string());
		}
		return false;
	});
	
	return directories;
}

std::set<asset_path> list_assets(const asset_path &path) {
	std::set<asset_path> assets;

	iterate_development_paths([&](fs::path &root_path) {
		root_path /= path;
		for (auto &item : fs::directory_iterator(root_path)) {
			if (!item.is_regular_file()) 
				continue;
			auto &path = item.path();
			if (path.extension() != ".json")
				continue;

			std::cout << std::filesystem::relative(
					item.path(), root_path).replace_extension("");

			assets.emplace(std::filesystem::relative(item.path(),
					root_path).replace_extension("").string());
		}
		return false;
	});
	
	return assets;
}

// void create(const asset_path &path, const std::string &type) {
// 	std::unique_ptr<asset> asset(type_to_constructor[type]());


// }

// void remove(const asset_path &path) {

// }

// void copy(const asset_path &src, const asset_path &dst) {

// }

// void move(const asset_path &src, const asset_path &dst) {
// 	copy(src, dst);
// 	remove(src);
// }

#endif

asset_ref<asset> load(const asset_path &path) {
	if (path_to_cached_owner.contains(path))
		return path_to_cached_owner[path];

	auto json_path_suffix = "assets/" + path + ".json";
#ifdef VOLT_DEVELOPMENT
	auto json_path = fs::path(VOLT_DEVELOPMENT_PATH) / json_path_suffix;

	if (!fs::exists(json_path)) {
		std::vector<std::string> paths = util::split(util::read_text_file(
				fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "paths.txt"), "\n");

		for (auto it = paths.rbegin(); it != paths.rend(); it++) {
			json_path = fs::path(it->substr(it->find(' ') + 1)) / json_path_suffix;
			
			if (fs::exists(json_path))
				break;
		}
	}
#else
	auto json_path = fs::current_path() / ".." / json_path_suffix;
#endif
	VOLT_ASSERT(fs::exists(json_path), "Invalid asset path:\n" + path)

	auto json = nl::json::parse(util::read_text_file(json_path));
	asset *asset = type_to_constructor[json["type"].get_ref<std::string &>()]();
	asset->deserialize(json["data"]);

	auto owner = new asset_owner(std::unique_ptr<assets::asset>(asset));
	owner->cache_it = path_to_cached_owner.emplace(std::make_pair(path, owner)).first;

	return owner;
}

void reload(const asset_path &path) {
	path_to_cached_owner[path]->ptr.reset();
	// path_to_cached_owner[path]->ptr = load(path);
}

// void reload() {
// 	for (auto &item : path_to_cached_owner)
// 		item.second->ptr->reload();
// }

const std::string &get_lang() {
	return lang;
}

void set_lang(const std::string &lang) {
	::lang = lang;
	// reload();
}

}

namespace volt::assets::_internal {

std::map<std::string, std::set<std::string>> module_name_to_types;
std::map<std::type_index, std::string> type_index_to_type;
std::map<std::string, std::function<asset *()>> type_to_constructor;
std::map<std::string, asset_owner *> path_to_cached_owner;

#ifdef VOLT_DEVELOPMENT

void module_load_callback(const std::string &module_name) {
	// There's no point in handling other modules,
	// because they won't ever be reloaded and
	// their state will never have to be recovered
	if (module_name != VOLT_DEVELOPMENT_MODULE)
		return;

	for (auto &type : module_name_to_types[VOLT_DEVELOPMENT_MODULE]) {
		for (auto &item : development_reload_snapshot[type]) {
			path_to_cached_owner[item.first]->ptr.reset(type_to_constructor[type]());
			path_to_cached_owner[item.first]->ptr->deserialize(item.second);
		}
	}

	development_reload_snapshot.clear();
}

#endif

void module_unload_callback(const std::string &module_name) {
	for (auto &type : module_name_to_types[module_name]) {
		type_to_constructor.erase(type);

		type_index_to_type.erase(std::find_if(
			type_index_to_type.begin(),
			type_index_to_type.end(),
			[&type](auto &item) {
				return item.second == type;
			}
		));

		for (auto &item : path_to_cached_owner) {
			if (type_index_to_type[typeid(*item.second->ptr.get())] != type)
				continue;

			// remember state
#ifdef VOLT_DEVELOPMENT
			if (module_name == VOLT_DEVELOPMENT_MODULE) {
				nl::json data = item.second->ptr->serialize();
				development_reload_snapshot[type][item.first] = data;
			}
#endif

			item.second->ptr.reset();
		}
	}

	module_name_to_types.erase(module_name);
}

}
