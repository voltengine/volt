#pragma once

#include <cstdint>
#include <memory>

namespace volt::gpu {

enum class api {
#ifdef VOLT_PLATFORM_WINDOWS
	d3d12,
#endif
#ifndef VOLT_PLATFORM_MACOS
	gl46,
#endif
#ifdef VOLT_PLATFORM_MACOS
	mtl,
#endif
#ifndef VOLT_PLATFORM_MACOS
	vk12,
#endif
};

class instance;

VOLT_API std::shared_ptr<gpu::instance> create_instance(gpu::api api);

}

#include "adapter.hpp"
#include "buffer.hpp"
#include "device.hpp"
#include "graph.hpp"
#include "instance.hpp"
#include "pass.hpp"
#include "shader.hpp"
#include "swapchain.hpp"
#include "texture.hpp"
