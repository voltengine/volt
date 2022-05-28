#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

namespace volt::util {

class version {
public:
	uint32_t major, minor, patch;

	version();

	version(uint32_t major, uint32_t minor, uint32_t patch,
			const std::vector<std::string> &pre_release,
			const std::vector<std::string> &build_metadata);

	version(const std::string &str);

	friend std::ostream &operator<<(std::ostream &lhs, const version &rhs);

	bool operator==(const version &rhs) const noexcept;

	bool operator!=(const version &rhs) const noexcept;

	bool operator<=(const version &rhs) const noexcept;

	bool operator>=(const version &rhs) const noexcept;

	bool operator<(const version &rhs) const noexcept;

	bool operator>(const version &rhs) const noexcept;

	static bool validate(const std::string &str);

	int32_t compare(const version &other) const noexcept;

	// Example use:
	// provided_version.backward_compatible(required_version)
	bool backward_compatible(const version &previous) const noexcept;

	const std::vector<std::string> &pre_release() const noexcept;

	void pre_release(const std::vector<std::string> &pre_release);

	const std::vector<std::string> &build_metadata() const noexcept;

	void build_metadata(const std::vector<std::string> &build_metadata);

private:
	std::vector<std::string> _pre_release, _build_metadata;
};

}
