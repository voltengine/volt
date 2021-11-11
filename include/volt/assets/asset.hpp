#pragma once

#include "../macros.hpp"

#include <string>

#include <nlohmann/json.hpp>

namespace volt::assets {

class asset {
public:
	virtual void deserialize(const nlohmann::json &json) {}

	virtual nlohmann::json serialize() const;

	//asset_ref<texture> get_thumbnail() const;

	// void save() const;

	// void reload() const;

	// void export_as(const std::filesystem::path &path);
};

}
