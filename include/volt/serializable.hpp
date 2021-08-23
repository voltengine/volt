#pragma once

#include "../macros.hpp"

#include <nlohmann/json.hpp>

namespace volt {

class serializable {
public:
	serializable() = default;

	virtual ~serializable() = default;

	virtual void deserialize(const nlohmann::json &) {} 

	virtual nlohmann::json serialize() const {
		return nlohmann::json::object();
	}
};

}
