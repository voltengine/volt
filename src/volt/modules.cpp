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
static std::unordered_map<std::string, module_handle> module_name_to_handle;

static fs::path name_to_path(const std::string &name) {
	return fs::current_path() / (name + volt::modules::module_extension);
}

namespace volt::modules {

static void run_load_callbacks(const std::string &name) {
	for (auto &item : _internal::module_name_to_load_callbacks) {
		for (auto &callback : item.second)
			callback(name);
	}
}

static void run_unload_callbacks(const std::string &name) {
	for (auto &item : _internal::module_name_to_unload_callbacks) {
		for (auto &callback : item.second)
			callback(name);
	}
}

// Use only after run_unload_callbacks
static void unload_without_erasing_name(const std::string &name) {
	_internal::module_name_to_unload_callbacks.erase(name);

	#ifdef VOLT_PLATFORM_LINUX
		dlclose(module_name_to_handle[name]);
	#elif defined(VOLT_PLATFORM_WINDOWS)
		FreeLibrary(module_name_to_handle[name]);
	#endif

	module_name_to_handle.erase(name);
}

static void load(const std::string &name, bool throw_errors) {
	std::string path = name_to_path(name).string();
	VOLT_LOG_INFO("Attempting to load " + name + ":\n" + path)

#ifdef VOLT_PLATFORM_LINUX
	module_handle handle = dlopen(path.c_str(), RTLD_NOW);
#elif defined(VOLT_PLATFORM_WINDOWS)
	module_handle handle = LoadLibrary(path.c_str());
#endif

	if (!handle) {
		VOLT_ASSERT(!throw_errors, "Failed to load " + name + " shared library.")
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
			VOLT_ASSERT(!throw_errors, "Failed to find volt_module_main in " + name + '.')
			return;
		}

		// Might always throw
		module_main();
	} catch (volt::error &e) {
		local_error = e;
	}
	
	if (local_error.has_value()) {
		// We can unload only outside of the try-catch
		run_unload_callbacks(name);
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
		
		std::string name = _internal::path_to_name(path.string());

#ifdef VOLT_DEVELOPMENT
		if (name == VOLT_DEVELOPMENT_MODULE)
			continue;
#endif

		try {
			load(name, false);
		} catch (volt::error &e) {
			volt::log::error(e.what(), e.where(), e.at());
		}
	}

#ifdef VOLT_DEVELOPMENT
	try {
		load(VOLT_DEVELOPMENT_MODULE, false);
	} catch (volt::error &e) {
		volt::log::error(e.what(), e.where(), e.at());
		VOLT_LOG_WARNING("Failed to load development module. "
				"Please correct the errors and attempt to reload.")
	}
#endif

	for (auto &name : module_names)
		run_load_callbacks(name);
}

void unload() {
	for (auto &name : module_names)
		run_unload_callbacks(name);

	for (auto &name : module_names)
		unload_without_erasing_name(name);

	module_names.clear();
}

#ifdef VOLT_DEVELOPMENT

void reload_development_module() {
	auto cache_path = fs::path(VOLT_DEVELOPMENT_PATH) / "cache";
	auto src = cache_path / "cmake" / "bin" / (std::string(VOLT_DEVELOPMENT_MODULE) + module_extension);
	auto dst = cache_path / "bin" / (std::string(VOLT_DEVELOPMENT_MODULE) + module_extension);
	auto backup_path = cache_path / (std::string("reload-backup") + module_extension);

	if (module_name_to_handle.contains(VOLT_DEVELOPMENT_MODULE)) {
		run_unload_callbacks(VOLT_DEVELOPMENT_MODULE);
		unload_without_erasing_name(VOLT_DEVELOPMENT_MODULE);
		module_names.erase(VOLT_DEVELOPMENT_MODULE);
		
		VOLT_LOG_INFO("Backing up development module...")
		fs::remove(backup_path);
		fs::copy(dst, backup_path);
	}

	fs::remove(dst);
	fs::copy(src, dst);
	
	try {
		load(VOLT_DEVELOPMENT_MODULE, true);
	} catch (volt::error &e) {
		volt::log::error(e.what(), e.where(), e.at());
		if (fs::exists(backup_path)) {
			VOLT_LOG_WARNING("Reload failed. Rolling back to backup...")
			fs::remove(dst);
			fs::copy(backup_path, dst);
			try {
				load(VOLT_DEVELOPMENT_MODULE, true);
			} catch (volt::error &e) {
				volt::log::error(e.what(), e.where(), e.at());
				VOLT_LOG_ERROR("Failed to load backup. Development module will remain unloaded.");
			}
		} else
			VOLT_LOG_ERROR("No backup available. Development module will remain unloaded.");
	}

	run_load_callbacks(VOLT_DEVELOPMENT_MODULE);
}

#endif

const std::set<std::string> &get_names() {
	return module_names;
}

}

namespace volt::modules::_internal {

std::unordered_map<std::string, std::vector<
		load_callback>> module_name_to_load_callbacks;
std::unordered_map<std::string, std::vector<
		unload_callback>> module_name_to_unload_callbacks;

}
