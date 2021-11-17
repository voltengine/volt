#include <iomanip>
#include <sstream>

namespace volt::util {

template<typename T>
std::string to_string(const T &value) {
	std::stringstream ss;
	ss << std::boolalpha << value;
	return ss.str();
}

// template<std::floating_point T>
// std::string to_string(T value, bool trim_zeros, int32_t precision) {
// 	std::stringstream ss;
// 	ss << std::fixed << std::setprecision(precision) << value;
	
// 	std::string str = ss.str();
// 	std::string trimmed = str;
// 	trimmed.erase(trimmed.find_last_not_of('0') + 1);

// 	if (trim_zeros) {
// 		str = trimmed;

// 		if (str == "-0.")
// 			str = "0";
// 		else if (str[str.size() - 1] == '.')
// 			str.pop_back();
// 	} else if (trimmed == "-0.")
// 		str.erase(str.begin());

// 	return str;
// }

}