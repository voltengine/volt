#include <volt/pch.hpp>
#include <volt/modules.hpp>

namespace fs = std::filesystem;

#ifdef VOLT_PLATFORM_LINUX
using module_handle = void *;
constexpr const char *module_extension = ".so";
#elif defined(VOLT_PLATFORM_WINDOWS)
using module_handle = HINSTANCE;
constexpr const char *module_extension = ".dll";
#endif

fs::path name_to_path(const std::string &name) {
	return fs::current_path() / (name + module_extension);
}

namespace volt::modules {

std::map<std::string, module_handle> handles;
std::vector<reload_callback> reload_callbacks;

void load(const std::string &name) {
	std::string path = name_to_path(name).string();

#ifdef VOLT_PLATFORM_LINUX
	module_handle handle = dlopen(path.c_str(), RTLD_NOW);
#elif defined(VOLT_PLATFORM_WINDOWS)
	module_handle handle = LoadLibrary(path.c_str());
#endif

	if (!handle)
		throw std::runtime_error("Failed to open module:\n" + path);

	using volt_module_main = void (*)();
#ifdef VOLT_PLATFORM_LINUX
	auto module_main = reinterpret_cast<volt_module_main>(dlsym(handle, "volt_module_main"));
#elif defined(VOLT_PLATFORM_WINDOWS)
	auto module_main = reinterpret_cast<volt_module_main>(GetProcAddress(handle, "volt_module_main"));
#endif

	if (!module_main)
		throw std::runtime_error("Module does not export volt_module_main:\n" + path);

	module_main();
	handles[name] = std::move(handle);
}

void unload(const std::string &name) {
	if (!handles.contains(name))
		throw std::runtime_error("No such module loaded: " + name);

#ifdef VOLT_PLATFORM_LINUX
	dlclose(handles[name]);
#elif defined(VOLT_PLATFORM_WINDOWS)
	FreeLibrary(handles[name]);
#endif

	handles.erase(name);
}

void reload(const std::string &name) {
	std::stack<load_callback> load_callbacks;
	for (auto &cb : reload_callbacks)
		load_callbacks.push(cb(name));

	unload(name);
	load(name);

	while (!load_callbacks.empty()) {
		load_callbacks.top()();
		load_callbacks.pop();
	}
}

void load_all() {
	for (auto &item : fs::directory_iterator(fs::current_path())) {
		auto &path = item.path();
		if (path.extension() != module_extension)
			continue;
		
		try {
			load(_path_to_name(path.string()));
		} catch (...) {}
	}
}

void unload_all() {
	// We use iterators to safely loop
	// while unlod() modifies the map
	for (auto it = handles.begin(); it != handles.end();) {
		auto prev = it++;
		unload(prev->first);
	}
}

void reload_all() {
	// As above
	for (auto it = handles.begin(); it != handles.end();) {
		auto prev = it++;
		reload(prev->first);
	}
}

void add_reload_callback(const reload_callback &callback) {
	reload_callbacks.emplace_back(std::move(callback));
}

void hello() {
	std::cout << volt::modules::this_module() << std::endl;
}

}
