#pragma once

#include "../../macros.hpp"

#include "../../assets/assets.hpp"
#include "../../ecs/ecs.hpp"

namespace volt::core::assets {

VOLT_ASSET class text : public volt::assets::asset {
public:
	std::string text;

	virtual void deserialize(const nlohmann::json &json) override;

	virtual nlohmann::json serialize() const override;
};

VOLT_COMPONENT class component {
public:
	int number;

	component() = default;

	component(const nlohmann::json &json) {
		number = json["number"];
	}

	operator nlohmann::json() const {
		auto json = nlohmann::json::object();
		json["number"] = number;
		return json;
	}
};

}
