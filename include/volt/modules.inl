#include <stdexcept>
#include <string>

#include "error.hpp"

#ifdef VOLT_PLATFORM_WINDOWS
	#include <Windows.h>
#elif defined(VOLT_PLATFORM_LINUX)
	#include <dlfcn.h>
#endif

namespace volt::modules {

std::string this_module_name() {
#ifdef VOLT_PLATFORM_LINUX

	Dl_info info;
	dladdr(reinterpret_cast<const void *>(&this_module_name), &info);

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

}

namespace volt::modules::_internal {

#ifdef VOLT_DEVELOPMENT

void register_development_module_load_callback(const std::function<void()> &callback) {
	_internal::module_name_to_development_module_load_callbacks[
			this_module_name()].push_back(callback);
}

void register_development_module_unload_callback(const std::function<void()> &callback) {
	_internal::module_name_to_development_module_unload_callbacks[
			this_module_name()].push_back(callback);
}

#endif

}
