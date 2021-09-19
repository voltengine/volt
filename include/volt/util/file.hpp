#pragma once

#include "../macros.hpp"

#include <filesystem>
#include <functional>
#include <string>
#include <string_view>

namespace volt::util {

VOLT_API std::string read_text_file(const std::filesystem::path &path);

VOLT_API void write_file(const std::filesystem::path &path, std::string_view str);

VOLT_API void shell(std::string cmd, const std::function<void(std::string_view)>
		&stdout_cb, bool redirect_stderr = true, size_t buffer_capacity = 1024);

}