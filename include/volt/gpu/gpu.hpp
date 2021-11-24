#pragma once

#include <cstdint>
#include <memory>

namespace volt::gpu {

enum class api {
	d3d12, vk12
};

class instance;

VOLT_API std::shared_ptr<gpu::instance> create_instance(gpu::api api);

}

#include "adapter.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "enums.hpp"
#include "instance.hpp"
#include "pool.hpp"
#include "routine.hpp"
#include "texture.hpp"