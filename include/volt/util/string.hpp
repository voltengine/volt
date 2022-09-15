#pragma once

#include "../macros.hpp"

#include <concepts>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace volt::util {

template<typename T>
std::string to_string(const T &value);

template<std::floating_point T>
std::string to_string(T value, bool trim_zeros = true, int32_t precision = 6);

VOLT_API void ltrim(std::string &str);

VOLT_API void rtrim(std::string &str);

VOLT_API void trim(std::string &str);

VOLT_API std::vector<std::string> split(std::string_view str,
		std::string_view delimiter, bool skip_empty = false);

VOLT_API void replace(std::string &str, std::string_view from, std::string_view to);

}

#include "string.inl"