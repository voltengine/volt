#pragma once

#if VOLT_IS_CURRENT_BUILD_TARGET
	#define VOLT_API __declspec(dllexport)
#else
	#define VOLT_API __declspec(dllimport)
#endif

namespace volt {

VOLT_API void hello();

}
