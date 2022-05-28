#pragma once

#include "macros.hpp"

#include <filesystem>

namespace volt::paths {

VOLT_API std::filesystem::path data();

VOLT_API std::filesystem::path documents();

VOLT_API std::filesystem::path pictures();

VOLT_API std::filesystem::path videos();

VOLT_API std::filesystem::path music();

VOLT_API std::filesystem::path saves();

}
