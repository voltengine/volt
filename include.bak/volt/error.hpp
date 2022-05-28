#pragma once

#include "macros.hpp"

#include <filesystem>
#include <stdexcept>
#include <string>

namespace volt {

class error : public std::exception {
public:
    VOLT_API error(std::string message, const std::filesystem::path &file, size_t line);

	virtual ~error() = default;

#ifdef VOLT_PLATFORM_LINUX
	VOLT_API const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
#else
	VOLT_API const char *what() const override;
#endif

	VOLT_API const std::filesystem::path &where() const;

	VOLT_API size_t at() const;

private:
	std::string message;
	std::filesystem::path file;
	size_t line;
};

}

#define VOLT_CUSTOM_ASSERT(condition, message, custom_error)\
		if (!(condition)) {\
			throw custom_error(message, __FILE__, __LINE__);\
		}

#define VOLT_ASSERT(condition, message)\
		VOLT_CUSTOM_ASSERT(condition, message, ::volt::error)

#ifdef VOLT_DEVELOPMENT
	#define VOLT_DEVELOPMENT_CUSTOM_ASSERT(condition, message, custom_error)\
			VOLT_CUSTOM_ASSERT(condition, message, custom_error)
	#define VOLT_DEVELOPMENT_ASSERT(condition, message)\
			VOLT_ASSERT(condition, message)
#else
	#define VOLT_DEVELOPMENT_CUSTOM_ASSERT(condition, message, custom_error)
	#define VOLT_DEVELOPMENT_ASSERT(condition, message)
#endif
