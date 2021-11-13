#pragma once

#include "../../macros.hpp"

#include <vector>

namespace volt::video::vk12::_internal {

VOLT_API extern std::vector<const char *> device_extensions;

VOLT_API const std::vector<const char *> &validation_layers();

}

#include "adapter.hpp"
#include "device.hpp"
#include "instance.hpp"
#include "shader.hpp"
#include "window.hpp"
