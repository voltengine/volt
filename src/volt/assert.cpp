#include <volt/pch.hpp>
#include <volt/assert.hpp>

namespace volt {

error::error(const std::string &message,
		const std::filesystem::path &file, size_t line)
		: message(message), file(file.filename()), line(line) {}

const std::string &error::what() const {
	return message;
}

const std::filesystem::path &error::where() const {
	return file;
}

size_t error::at() const {
	return line;
}

}
