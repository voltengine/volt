#pragma once

#include "../macros.hpp"

#include <stdexcept>

namespace volt {

// void _log(const std::string &message,
// 		const std::filesystem::path &file,
// 		size_t line, decltype(termcolor::reset)) {
// 	message += " (" + file.filename().string() + ':' + line + ")";
// 	std::cout << termcolor::bright_green << message << termcolor::reset;
// }

}

#define VOLT_LOG_INFO(message) \
		::volt::_log_info( \
				message, __FILE__, __LINE__));
#define VOLT_LOG_WARNING(message) \
		::volt::_log_warning( \
				message, __FILE__, __LINE__));
#define VOLT_LOG_ERROR(message) \
		::volt::_log_error( \
				message, __FILE__, __LINE__));

#ifdef VOLT_DEVELOPMENT
	#define VOLT_DEVELOPMENT_LOG_INFO VOLT_LOG_INFO
	#define VOLT_DEVELOPMENT_LOG_WARNING VOLT_LOG_WARNING
	#define VOLT_DEVELOPMENT_LOG_ERROR VOLT_LOG_ERROR
#else
	#define VOLT_DEVELOPMENT_ASSERT
#endif