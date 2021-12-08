#pragma once

#include "../macros.hpp"

#include <chrono>

namespace volt::util {

class timer {
public:
	VOLT_API timer();

	VOLT_API bool running() const;

	VOLT_API void start();

	VOLT_API void stop();

	VOLT_API void reset();

	VOLT_API double elapsed() const;

private:
	bool _running;
	std::chrono::high_resolution_clock::time_point start_time, end_time;
};

}
