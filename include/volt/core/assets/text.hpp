#pragma once

#include "../../macros.hpp"

#include "../../assets/assets.hpp"

namespace volt::core::assets {

VOLT_ASSET class text : public volt::assets::asset {
public:
	std::string text;

	virtual void deserialize(const nlohmann::json &json) override;

	virtual nlohmann::json serialize() const override;
};

}
