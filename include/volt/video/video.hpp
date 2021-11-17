#pragma once

#include <cstdint>
#include <memory>

namespace volt::video {

enum class api {
	vk12
};

class instance;

VOLT_API std::shared_ptr<video::instance> create_instance(video::api api);

}

#include "adapter.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "enums.hpp"
#include "instance.hpp"
#include "pool.hpp"
#include "queue.hpp"
#include "routine.hpp"
#include "texture.hpp"
