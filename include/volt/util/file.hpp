#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <filesystem>
#include <functional>
#include <string>
#include <string_view>
#include <vector>

namespace volt::util {

VOLT_API std::string read_text_file(const std::filesystem::path &path);

VOLT_API std::vector<uint8_t> read_binary_file(const std::filesystem::path &path);

VOLT_API void write_file(const std::filesystem::path &path, std::string_view str);

VOLT_API void write_file(const std::filesystem::path &path, const std::vector<uint8_t> &buffer);

VOLT_API void shell(std::string cmd, const std::function<void(std::string_view)>
		&stdout_cb, bool redirect_stderr = true, size_t buffer_capacity = 1024);

}