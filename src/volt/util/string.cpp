#include <volt/pch.hpp>
#include <volt/util/string.hpp>

namespace util {

void ltrim(std::string &str) {
	auto end = std::find_if(str.begin(), str.end(),
			[](char c) { return !std::isspace(c); });
	str.erase(str.begin(), end);
}

void rtrim(std::string &str) {
	auto begin = std::find_if(str.rbegin(), str.rend(),
			[](char c) { return !std::isspace(c); }).base();
    str.erase(begin, str.end());
}

void trim(std::string &str) {
	ltrim(str);
	rtrim(str);
}

std::vector<std::string> split(std::string_view str,
		std::string_view delimiter, bool skip_empty) {
	std::vector<std::string> tokens;
	size_t pos = 0, len = str.size();

	while (pos <= len) {
		size_t new_pos = std::min(str.find(delimiter, pos), len);
		if (!skip_empty || new_pos != pos)
			tokens.push_back(std::string(str.substr(pos, new_pos - pos)));
		pos = new_pos + 1;
	}

	return tokens;
}

void replace(std::string &str, std::string_view from, std::string_view to) {
	size_t pos = 0;

	while((pos = str.find(from, pos)) != std::string::npos) {
		str.replace(pos, from.size(), to);
		pos += to.size();
	}
}

}