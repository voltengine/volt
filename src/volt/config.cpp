#include <volt/pch.hpp>
#include <volt/config.hpp>

#include <volt/util/util.hpp>
#include <volt/log.hpp>
#include <volt/paths.hpp>

namespace fs = std::filesystem;
namespace nl = nlohmann;

#ifdef VOLT_DEVELOPMENT
static nl::json base_config;
#endif

static nl::json default_config, config;

static nl::json cleave(nl::json from, const nl::json &what) {
	// Cleave from + what
	std::stack<std::pair<nl::json &, const nl::json &>> objects_to_cleave;
	objects_to_cleave.emplace(from, what);

	while (!objects_to_cleave.empty()) {
		nl::json &from = objects_to_cleave.top().first;
		const nl::json &what = objects_to_cleave.top().second;
		objects_to_cleave.pop();
		
		for (auto it = from.begin(); it != from.end();) {
			auto prev = it++;

			if (what.contains(prev.key())) {
				nl::json &from_v = prev.value();
				const nl::json &what_v = what[prev.key()];

				if (from_v == what_v)
					from.erase(prev);
				else if (from_v.is_object() && what_v.is_object())
					objects_to_cleave.emplace(from_v, what_v);
			}
		}
	}

	return from;
}

namespace volt::config {

static void load_from_data() {
	try {
		::config.update(nl::json::parse(util::read_text_file(
				paths::data() / "config.json")));
		VOLT_LOG_INFO("User configuration has been loaded.")
	} catch (...) {
		VOLT_LOG_INFO("No user configuration available.")
	}
}

static void try_init() {
	static bool initialized = false;
	if (initialized)
		return;
	initialized = true;

#ifdef VOLT_DEVELOPMENT
	std::vector<std::string> paths = util::split(util::read_text_file(
			fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "paths.txt"), "\n");

	base_config = nl::json::object();
	for (auto &path_item : paths) {
		auto path = path_item.substr(path_item.find(' ') + 1) / fs::path("config.json");
		if (fs::exists(path))
			base_config.update(nl::json::parse(util::read_text_file(path)));
	}

	default_config = base_config;
	auto path = fs::path(VOLT_DEVELOPMENT_PATH) / "config.json";
	if (fs::exists(path))
		default_config.update(nl::json::parse(util::read_text_file(path)));
#else
	default_config = nl::json::parse(util::read_text_file(
			fs::current_path() / ".." / "config.json"));
#endif

	::config = default_config;

#ifndef VOLT_DEVELOPMENT
	load_from_data();
#endif
}

nl::json &json() {
	try_init();
	return ::config;
}

void save() {
	try_init();
	nl::json cleaved = cleave(::config, default_config);
	util::write_file(paths::data() / "config.json", cleaved.dump(1, '\t'));
}

void reset_to_default() {
	try_init();
	::config = default_config;
}

#ifdef VOLT_DEVELOPMENT

void save_default() {
	try_init();
	nl::json cleaved = cleave(default_config, base_config);
	util::write_file(fs::path(VOLT_DEVELOPMENT_PATH) /
			"config.json", cleaved.dump(1, '\t'));
}

void reset_to_base() {
	try_init();
	::config = default_config = base_config;
}

void load() {
	load_from_data();
}

#endif

}
