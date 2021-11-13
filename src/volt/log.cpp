#include <volt/pch.hpp>
#include <volt/log.hpp>

#include <volt/util/util.hpp>
#include <volt/config.hpp>
#include <volt/paths.hpp>

namespace fs = std::filesystem;
namespace tc = termcolor;

namespace volt::log {

void _log(const std::string &prefix, std::string message, const fs::path &file, size_t line) {
	using namespace std::chrono;

	message += " (" + file.filename().string() + ':' + util::to_string(line) + ")";
	std::cout << message << tc::reset << std::endl;

	// We must init config before initializing static variable below, because
	// it might call _log(...), i.e. this function during fist call to its API 
	config::json();

	static auto log = []() {
		auto now = system_clock::now();
    	std::string iso_day = date::format("%F", now);

		fs::path logs_path = paths::data() / "logs";
		fs::path log_path;

		uint32_t index = 0;
		do {
			log_path = logs_path / (iso_day + '-' + util::to_string(++index) + ".log");
		} while (fs::exists(log_path));

		fs::create_directories(logs_path);
		return std::ofstream(log_path, std::ios_base::out);
	}();

	auto now = floor<seconds>(system_clock::now());
	std::string timestamp = date::format("%T", now);

    log << '[' << timestamp << "] " << prefix << message << std::endl;
}

void info(const std::string &message, const fs::path &file, size_t line) {
	_log("[INFO]: ", message, file, line);
}

void warning(const std::string &message, const fs::path &file, size_t line) {
	std::cout << tc::bright_yellow;
	_log("[WARNING]: ", message, file, line);
}

void error(const std::string &message, const fs::path &file, size_t line) {
	std::cout << tc::bright_red;
	_log("[ERROR]: ", message, file, line);
}

}
