#pragma once

#include "macros.hpp"

#include <filesystem>
#include <iostream>

namespace volt::log {

VOLT_API void _log(const std::string &prefix, std::string message,
		const std::filesystem::path &file, size_t line);

VOLT_API void info(const std::string &message,
		const std::filesystem::path &file, size_t line);

VOLT_API void warning(const std::string &message,
		const std::filesystem::path &file, size_t line);

VOLT_API void error(const std::string &message,
		const std::filesystem::path &file, size_t line);

}

#define VOLT_LOG_INFO(message)\
		::volt::log::info(message, __FILE__, __LINE__);
#define VOLT_LOG_WARNING(message)\
		::volt::log::warning(message, __FILE__, __LINE__);
#define VOLT_LOG_ERROR(message)\
		::volt::log::error(message, __FILE__, __LINE__);

#ifdef VOLT_DEVELOPMENT
	#define VOLT_DEVELOPMENT_LOG_INFO(message) VOLT_LOG_INFO(message)
	#define VOLT_DEVELOPMENT_LOG_WARNING(message) VOLT_LOG_WARNING(message)
	#define VOLT_DEVELOPMENT_LOG_ERROR(message) VOLT_LOG_ERROR(message)
#else
	#define VOLT_DEVELOPMENT_LOG_INFO(message)
	#define VOLT_DEVELOPMENT_LOG_WARNING(message)
	#define VOLT_DEVELOPMENT_LOG_ERROR(message)
#endif