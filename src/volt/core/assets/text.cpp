#include <volt/pch.hpp>
#include <volt/core/assets/text.hpp>

namespace nl = nlohmann;

namespace volt::core::assets {

void text::deserialize(const nl::json &json) {
	text = json.get<std::string>();
}

nl::json text::serialize() const {
	return text;
}

}
