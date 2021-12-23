  
#include <volt/pch.hpp>
#include <volt/util/file.hpp>

#include <volt/error.hpp>

namespace fs = std::filesystem;

namespace volt::util {

std::string read_text_file(const fs::path &path) {
	VOLT_ASSERT(fs::exists(path), "File not found:\n" + path.string())

	std::ifstream stream(path, std::ifstream::in);
	return std::string(
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>()
	);
}

std::vector<uint8_t> read_binary_file(const fs::path &path) {
	VOLT_ASSERT(fs::exists(path), "File not found:\n" + path.string())

	std::ifstream stream(path, std::ifstream::in | std::ifstream::binary);
	return std::vector<uint8_t>(
		std::istreambuf_iterator<char>(stream),
		std::istreambuf_iterator<char>()
	);
}

void write_file(const fs::path &path, std::string_view str) {
	fs::create_directories(path.parent_path());
	std::ofstream stream(path, std::ofstream::out);
	stream.write(str.data(), str.size());
}

void write_file(const fs::path &path, const std::vector<uint8_t> &buffer) {
	fs::create_directories(path.parent_path());
	std::ofstream stream(path, std::ofstream::out | std::ofstream::binary);
    stream.write(reinterpret_cast<const char *>(buffer.data()), buffer.size());
}

void shell(std::string cmd, const std::function<void(std::string_view)>
		&stdout_cb, bool redirect_stderr, size_t buffer_capacity) {
    std::vector<char> buffer;
	buffer.resize(buffer_capacity);

	if (redirect_stderr)
		cmd += " 2>&1";

#ifdef VOLT_PLATFORM_LINUX
	std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
#elif defined(VOLT_PLATFORM_WINDOWS)
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd.c_str(), "r"), _pclose);
#endif

    VOLT_ASSERT(pipe, "Failed to execute command.")
	
    while (std::fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
        stdout_cb(buffer.data());
}

}