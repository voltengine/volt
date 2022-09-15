#include <volt/pch.hpp>
#include <volt/util/timer.hpp>

using namespace std::chrono;

namespace volt::util {

timer::timer() : _running(true) {
	reset();
}

bool timer::running() const {
	return _running;
}

void timer::start() {
	if (!_running)
		start_time += steady_clock::now() - end_time;
	_running = true;
}

void timer::stop() {
	if (_running)
		end_time = steady_clock::now();
	_running = false;
}

void timer::reset() {
	start_time = end_time = steady_clock::now();
}

double timer::elapsed() const {
	return duration_cast<duration<double>>((
			_running ? steady_clock::now()
			: end_time) - start_time).count();
}

}
