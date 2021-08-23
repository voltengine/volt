#include <stdexcept>
#include <string>

namespace volt::modules {

inline std::string _path_to_name(const std::string &path) {
#ifdef VOLT_PLATFORM_LINUX
	size_t filename_index = path.rfind('/') + 1;
#elif defined(VOLT_PLATFORM_WINDOWS)
	size_t filename_index = path.rfind('\\') + 1;
#endif

	size_t dot_index = path.rfind('.');
	return path.substr(filename_index, dot_index - filename_index);
}

std::string this_module() {
#ifdef VOLT_PLATFORM_LINUX

	Dl_info info;
	link_map *current_map, *executable_map;

	dladdr1(&this_module, &info, &current_map, RTLD_DL_LINKMAP);
	dlinfo(dlopen(nullptr), RTLD_DI_LINKMAP, &executable_map);

	if (current_map == executable_map)
		throw std::runtime_error("Cannot query module name from executable.");

	std::string path = info.dli_fname;
	return _path_to_name(path);

#elif defined(VOLT_PLATFORM_WINDOWS)

	HMODULE handle = nullptr;

	std::string path;
	path.resize(MAX_PATH);

	if (!GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCSTR>(&this_module), &handle)) {
		throw std::runtime_error("GetModuleHandleEx returned "
				+ std::to_string(GetLastError()) + '.');
	}

	if (handle == GetModuleHandle(nullptr))
		throw std::runtime_error("Cannot query module name from executable.");

	if (!GetModuleFileName(handle, path.data(), path.size())) {
		throw std::runtime_error("GetModuleFileName returned "
				+ std::to_string(GetLastError()) + '.');
	}

	return _path_to_name(path);

#endif
}

}