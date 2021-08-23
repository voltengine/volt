#pragma once

#include "macros.hpp"

#include <stdexcept>

#ifdef VOLT_DEVELOPMENT
	#define VOLT_ASSERT(condition, message) \
			if (!(condition)) throw volt::assertion_error(message);
#else
	#define VOLT_ASSERT
#endif

namespace volt {
	using assertion_error = std::runtime_error;
}
