#include <volt/pch.hpp>
#include <volt/modules/modules.hpp>

#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
namespace nl = nlohmann;

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

std::map<std::string, module_handle> module_name_to_handle;
std::map<std::string, std::set<std::string>
		> _module_name_to_serializable_names;
std::map<std::string, std::function<serializable *
		()>> _serializable_name_to_constructor;
std::map<std::string, std::list<_instance_owner *
		>> _serializable_name_to_instance_tracking_list;

void load(const std::string &name) {
	std::string path = name_to_path(name).string();

#ifdef VOLT_PLATFORM_LINUX
	module_handle handle = dlopen(path.c_str(), RTLD_NOW);
#elif defined(VOLT_PLATFORM_WINDOWS)
	module_handle handle = LoadLibrary(path.c_str());
#endif

	if (!handle)
		throw std::runtime_error("Failed to load module:\n" + path);

	using volt_module_load = void (*)();
#ifdef VOLT_PLATFORM_LINUX
	auto module_load = reinterpret_cast<volt_module_load>(dlsym(handle, "volt_module_load"));
#elif defined(VOLT_PLATFORM_WINDOWS)
	auto module_load = reinterpret_cast<volt_module_load>(GetProcAddress(handle, "volt_module_load"));
#endif

	if (!module_load)
		throw std::runtime_error("Module does not export volt_module_load:\n" + path);
	module_load();

	module_name_to_handle[name] = std::move(handle);
}

void unload(const std::string &name) {
	if (!module_name_to_handle.contains(name))
		throw std::runtime_error("No such module loaded: " + name);

	auto handle = module_name_to_handle[name];

	using volt_module_unload = void (*)();
	#ifdef VOLT_PLATFORM_LINUX
		auto module_unload = reinterpret_cast<volt_module_unload>(dlsym(handle, "volt_module_unload"));
	#elif defined(VOLT_PLATFORM_WINDOWS)
		auto module_unload = reinterpret_cast<volt_module_unload>(GetProcAddress(handle, "volt_module_unload"));
	#endif

	if (!module_unload)
		throw std::runtime_error("Module does not export volt_module_unload:\n"
				+ name_to_path(name).string());
	module_unload();

#ifdef VOLT_PLATFORM_LINUX
	dlclose(handle);
#elif defined(VOLT_PLATFORM_WINDOWS)
	FreeLibrary(handle);
#endif

	module_name_to_handle.erase(name);
}

void reload(const std::vector<std::string> &names, const std::function<void()> &callback) {
	// Serializable name -> tracking list + list of JSON values
	std::map<std::string, std::pair<std::list<
			_instance_owner *>, std::vector<nl::json>>> data;

	// Serialize and delete all serializable instances
	for (auto &module_name : names) {
		if (!module_name_to_handle.contains(module_name))
			throw std::runtime_error("No such module loaded: " + module_name);

		auto &serializable_names = _module_name_to_serializable_names[module_name];

		for (auto &serializable_name : serializable_names) {
			auto &serializable_data = data[serializable_name];
			serializable_data.first = std::move(
					_serializable_name_to_instance_tracking_list[serializable_name]);

			for (auto owner : serializable_data.first)
				serializable_data.second.emplace_back(owner->ptr->serialize());
		}
	}	

	// Unload to delete instances and constructors
	for (auto &module_name : names)
		unload(module_name);
	
	// Call back when all modules are down
	callback();

	// Load to register new constructors
	for (auto &module_name : names)
		load(module_name);

	// Deserialize data to new instances
	for (auto &module_name : names) {
		auto &serializable_names = _module_name_to_serializable_names[module_name];

		for (auto &serializable_name : serializable_names) {
			if (!_serializable_name_to_constructor
					.contains(serializable_name))
				continue;

			auto &serializable_data = data[serializable_name];

			size_t i = 0;
			for (auto owner : serializable_data.first) {
				owner->ptr = _serializable_name_to_constructor[serializable_name]();
				owner->ptr->deserialize(serializable_data.second[i++]);
			}

			_serializable_name_to_instance_tracking_list
					[serializable_name] = std::move(serializable_data.first);
		}
	}
}

void load() {
	for (auto &item : fs::directory_iterator(fs::current_path())) {
		auto &path = item.path();
		if (path.extension() != module_extension)
			continue;
		
		try {
			load(_path_to_name(path.string()));
		} catch (...) {}
	}
}

void unload() {
	// We use iterators to safely loop
	// while unlod() modifies the map
	for (auto it = module_name_to_handle.begin();
			it != module_name_to_handle.end();) {
		auto prev = it++;
		unload(prev->first);
	}
}

void reload(const std::function<void()> &callback) {
	auto names = std::accumulate(
			module_name_to_handle.begin(),
			module_name_to_handle.end(),
			std::vector<std::string>(),
			[](auto &&accumulator, auto &item) {
		accumulator.push_back(item.first);
        return std::move(accumulator);
    });
	reload(names, callback);
}

void unregister_serializable(const std::string &name) {
	_module_name_to_serializable_names[this_module()].erase(name);
	_serializable_name_to_constructor[name] = nullptr;

	for (auto owner : _serializable_name_to_instance_tracking_list[name]) {
		delete owner->ptr;
		owner->ptr = nullptr;
	}
	_serializable_name_to_instance_tracking_list.erase(name);
}

shared_instance<> instantiate_serializable(const std::string &name) {
	auto &tracking_list = _serializable_name_to_instance_tracking_list[name];

	auto *ptr = _serializable_name_to_constructor[name]();
	auto *owner = new _instance_owner(ptr, tracking_list);
	return shared_instance<>(owner);
}

void hello() {
	std::cout << volt::modules::this_module() << std::endl;
}

}
