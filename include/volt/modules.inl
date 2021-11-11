#include <stdexcept>
#include <string>

#include "error.hpp"

#ifdef VOLT_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

namespace volt::modules {

std::string this_module_name() {
#ifdef VOLT_PLATFORM_LINUX

	Dl_info info;
	dladdr(&this_module, &info);

	std::string path = info.dli_fname;
	return _internal::path_to_name(path);

#elif defined(VOLT_PLATFORM_WINDOWS)

	HMODULE handle = nullptr;

	std::string path;
	path.resize(MAX_PATH);

	VOLT_ASSERT(GetModuleHandleEx(
			GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			reinterpret_cast<LPCSTR>(&this_module_name), &handle),
			"GetModuleHandleEx returned " + std::to_string(GetLastError()) + '.');

	VOLT_ASSERT(GetModuleFileName(handle, path.data(), path.size()),
			"GetModuleFileName returned " + std::to_string(GetLastError()) + '.');

	return _internal::path_to_name(path);

#endif
}

void register_load_callback(const load_callback &callback) {
	_internal::module_name_to_load_callbacks[
			this_module_name()].push_back(callback);
}

void register_unload_callback(const unload_callback &callback) {
	_internal::module_name_to_unload_callbacks[
			this_module_name()].push_back(callback);
}

}

namespace volt::modules::_internal {

inline std::string path_to_name(const std::string &path) {
#ifdef VOLT_PLATFORM_LINUX
	size_t filename_index = path.rfind('/') + 1;
#elif defined(VOLT_PLATFORM_WINDOWS)
	size_t filename_index = path.rfind('\\') + 1;
#endif

	size_t dot_index = path.rfind('.');
	return path.substr(filename_index, dot_index - filename_index);
}

}
