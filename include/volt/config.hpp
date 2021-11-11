#pragma once

#include "macros.hpp"

#include <nlohmann/json.hpp>

namespace volt::config {

VOLT_API nlohmann::json &json();

VOLT_API void save();

VOLT_API void reset_to_default();

#ifdef VOLT_DEVELOPMENT

VOLT_API void save_default();

VOLT_API void reset_to_base();

VOLT_API void load();

#endif

}
