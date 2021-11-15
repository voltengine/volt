#pragma once

#include <memory>

namespace volt::video {

enum class api {
	vk12
};









class instance;

VOLT_API std::shared_ptr<video::instance> create_instance(video::api api);

}

#include "adapter.hpp"
#include "device.hpp"
#include "instance.hpp"
#include "shader.hpp"
