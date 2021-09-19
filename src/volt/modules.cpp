#include <volt/pch.hpp>
#include <volt/modules.hpp>

#include <nlohmann/json.hpp>

#include <volt/error.hpp>
#include <volt/log.hpp>

namespace fs = std::filesystem;
namespace nl = nlohmann;

#ifdef VOLT_PLATFORM_LINUX
using module_handle = void *;
#elif defined(VOLT_PLATFORM_WINDOWS)
using module_handle = HINSTANCE;
#endif

static std::set<std::string> module_names;
static std::map<std::string, module_handle> module_name_to_handle;

static fs::path name_to_path(const std::string &name) {
	return fs::current_path() / (name + volt::modules::module_extension);
}

namespace volt::modules {

std::map<std::string, std::vector<
		unload_callback>> _module_name_to_unload_callbacks;

static void run_unload_callbacks(const std::string &name, bool remember_state) {
	for (auto &item : _module_name_to_unload_callbacks) {
		for (auto &callback : item.second)
			callback(name, remember_state);
	}
}

static void unload_without_erasing_name(const std::string &name) {
	_module_name_to_unload_callbacks.erase(name);

	#ifdef VOLT_PLATFORM_LINUX
		dlclose(module_name_to_handle[name]);
	#elif defined(VOLT_PLATFORM_WINDOWS)
		FreeLibrary(module_name_to_handle[name]);
	#endif

	module_name_to_handle.erase(name);
}

static void load(const std::string &name, bool throw_errors) {
	std::string path = name_to_path(name).string();

#ifdef VOLT_PLATFORM_LINUX
	module_handle handle = dlopen(path.c_str(), RTLD_NOW);
#elif defined(VOLT_PLATFORM_WINDOWS)
	module_handle handle = LoadLibrary(path.c_str());
#endif

	if (!handle) {
		VOLT_ASSERT(!throw_errors, "Failed to load module:\n" + path)
		return;
	}

	// This is needed for emergency unload
	module_name_to_handle[name] = std::move(handle);

	// We must copy any potential exception from module_main
	// because it's owned by the module
	std::optional<volt::error> local_error;
	try {
		using volt_module_main = void (*)();
#ifdef VOLT_PLATFORM_LINUX
		auto module_main = reinterpret_cast<volt_module_main>(dlsym(handle, "volt_module_load"));
#elif defined(VOLT_PLATFORM_WINDOWS)
		auto module_main = reinterpret_cast<volt_module_main>(GetProcAddress(handle, "volt_module_main"));
#endif

		if (!module_main) {
			VOLT_ASSERT(!throw_errors, "Module does not export volt_module_main:\n" + path)
			return;
		}

		// Might always throw
		module_main();
	} catch (volt::error &e) {
		local_error = e;
	}
	
	if (local_error.has_value()) {
		// We can unload only outside of the try-catch
		run_unload_callbacks(name, false);
		unload_without_erasing_name(name);
		throw local_error.value();
	}

	module_names.emplace(name);
}

void load() {
	for (auto &item : fs::directory_iterator(fs::current_path())) {
		auto &path = item.path();
		if (path.extension() != module_extension)
			continue;

		std::string name = _path_to_name(path.string());

		try {
			load(name, false);
		} catch (volt::error &e) {
			volt::log::error("Failed to load \"" + name + "\":\n" +
					e.what(), e.where(), e.at());
		}
	}
}

void unload() {
	for (auto &name : module_names)
		run_unload_callbacks(name, false);

	for (auto &name : module_names)
		unload_without_erasing_name(name);

	module_names.clear();
}

#ifdef VOLT_DEVELOPMENT
void reload_development_module(const std::function<void()> &callback) {
	if (module_name_to_handle.contains(VOLT_DEVELOPMENT_MODULE)) {
		run_unload_callbacks(VOLT_DEVELOPMENT_MODULE, true);
		unload_without_erasing_name(VOLT_DEVELOPMENT_MODULE);
		module_names.erase(VOLT_DEVELOPMENT_MODULE);
	}

	callback();

	
	try {
		load(VOLT_DEVELOPMENT_MODULE, true);
	} catch (volt::error &e) {
		volt::log::error("Failed to load \"" VOLT_DEVELOPMENT_MODULE "\":\n" +
				e.what(), e.where(), e.at());
	}
}
#endif

const std::set<std::string> &get_names() {
	return module_names;
}

}
