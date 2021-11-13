#include <volt/pch.hpp>
#include <volt/util/version.hpp>

#include <algorithm>
#include <regex>

#include <volt/util/string.hpp>

namespace volt::util {

version::version() : major(0), minor(1), patch(0) {}

version::version(uint32_t major, uint32_t minor, uint32_t patch,
		const std::vector<std::string> &pre_release,
		const std::vector<std::string> &build_metadata)
		: major(major), minor(minor), patch(patch) {
	set_pre_release(pre_release);
	set_build_metadata(build_metadata);
}

version::version(const std::string &str) {
	static const std::regex parser("^(0|[1-9]\\d*)\\.(0|[1-9]"
			"\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|"
			"\\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]"
			"\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:"
			"\\+([0-9a-zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$");
	std::smatch match;

	if (!std::regex_match(str, match, parser))
		throw std::invalid_argument("String is not a valid semantic version: " + str);

	major = std::stoul(match.str(1));
	minor = std::stoul(match.str(2));
	patch = std::stoul(match.str(3));
	pre_release = util::split(match.str(4), ".", true);
	build_metadata = util::split(match.str(5), ".", true);
}

std::ostream &operator<<(std::ostream &lhs, const version &rhs) {
	lhs << rhs.major << '.' << rhs.minor << '.' << rhs.patch;

	if (rhs.pre_release.size() > 0) {
		lhs << '-' << std::accumulate(rhs.pre_release.begin() + 1,
				rhs.pre_release.end(), rhs.pre_release[0],
				[](std::string &&str, const std::string &identifier) {
					return std::move(str) + '.' + identifier;
				});
	}

	if (rhs.build_metadata.size() > 0) {
		lhs << '+' << std::accumulate(rhs.build_metadata.begin() + 1,
				rhs.build_metadata.end(), rhs.build_metadata[0],
				[](std::string &&str, const std::string &identifier) {
					return std::move(str) + '.' + identifier;
				});
	}

	return lhs;
}

bool version::operator==(const version &rhs) const noexcept {
	return compare(rhs) == 0;
}

bool version::operator!=(const version &rhs) const noexcept {
	return compare(rhs) != 0;
}

bool version::operator<=(const version &rhs) const noexcept {
	return compare(rhs) <= 0;
}

bool version::operator>=(const version &rhs) const noexcept {
	return compare(rhs) >= 0;
}

bool version::operator<(const version &rhs) const noexcept {
return compare(rhs) < 0;
}

bool version::operator>(const version &rhs) const noexcept {
	return compare(rhs) > 0;
}

static bool validate(const std::string &str) {
	static const std::regex validator("^(0|[1-9]\\d*)\\.(0|[1-9]"
			"\\d*)\\.(0|[1-9]\\d*)(?:-((?:0|[1-9]\\d*|"
			"\\d*[a-zA-Z-][0-9a-zA-Z-]*)(?:\\.(?:0|[1-9]"
			"\\d*|\\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(?:"
			"\\+([0-9a-zA-Z-]+(?:\\.[0-9a-zA-Z-]+)*))?$");

	return std::regex_match(str, validator);
}

int32_t version::compare(const version &other) const noexcept {
	if (major != other.major)
		return major > other.major ? 1 : -1;
	
	if (minor != other.minor)
		return minor > other.minor ? 1 : -1;

	if (patch != other.patch)
		return patch > other.patch ? 1 : -1;

	size_t a_size = pre_release.size();
	size_t b_size = other.pre_release.size();

	size_t min_size = std::min(a_size, b_size);
	if (min_size == 0)
		return a_size == 0 ? 1 : -1;

	for (size_t i = 0; i < min_size; i++) {
		auto &a = pre_release[i];
		auto &b = other.pre_release[i];

		// These won't throw
		bool a_numeric = std::all_of(a.begin(), a.end(), [](auto &c) noexcept { return std::isdigit(c); });
		bool b_numeric = std::all_of(b.begin(), b.end(), [](auto &c) noexcept { return std::isdigit(c); });

		if (a_numeric) {
			if (b_numeric) {
				uint32_t a_num = std::stoul(a);
				uint32_t b_num = std::stoul(b);

				if (a_num != b_num)
					return a_num > b_num ? 1 : -1;
			} else
				return -1;
		} else {
			if (b_numeric)
				return 1;
			else {
				int32_t comparison = a.compare(b);
				if (comparison != 0)
					return comparison > 0 ? 1 : -1;
			}
		}
	}

	if (a_size != b_size)
		return a_size > b_size ? 1 : -1;

	return 0;
}

bool version::is_backward_compatible(const version &previous) const noexcept {
	// Example use:
	// provided_version.is_backward_compatible(required_version)

	if (major != previous.major)
		return false;
	return previous < *this;
}

const std::vector<std::string> &version::get_pre_release() const noexcept {
	return pre_release;
}

void version::set_pre_release(const std::vector<std::string> &pre_release) {
	static const std::regex validator("[0-9A-Za-z-]+");

	for (const std::string &identifier : pre_release) {
		bool is_numeric = std::all_of(identifier.begin(),
				identifier.end(), [](auto &c) { return std::isdigit(c); });
		
		if (identifier.empty() ||
				(!is_numeric && !std::regex_match(identifier, validator)) ||
				(is_numeric && identifier.size() > 1 && identifier[0] == '0')) {
			throw std::invalid_argument("Identifiers must comprise"
					" only ASCII alphanumerics and hyphens."
					" Identifiers must not be empty."
					" Numeric identifiers must not include leading zeroes.");
		}
	}

	this->pre_release = pre_release;
}

const std::vector<std::string> &version::get_build_metadata() const noexcept {
	return build_metadata;
}

void version::set_build_metadata(const std::vector<std::string> &build) {
	static const std::regex validator("[0-9A-Za-z-]+");

	for (std::string &identifier : build_metadata) {
		if (identifier.empty() || !std::regex_match(identifier, validator)) {
			throw std::invalid_argument("Identifiers must comprise"
					" only ASCII alphanumerics and hyphens."
					" Identifiers must not be empty.");
		}
	}

	this->build_metadata = build_metadata;
}

}
