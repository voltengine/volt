#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "adapter.hpp"
#include "buffer.hpp"
#include "enums.hpp"
#include "pool.hpp"
#include "resource.hpp"
#include "swapchain.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device : public std::enable_shared_from_this<device> {
public:
	virtual ~device() = default;

	virtual void wait() = 0;

	virtual std::shared_ptr<gpu::buffer> create_buffer(
			gpu::memory_type memory_type,
			gpu::buffer_features features,
			size_t size) = 0;

	virtual std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) = 0;

	virtual std::shared_ptr<gpu::texture> create_2d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			math::uvec2 size, uint32_t levels) = 0;

	VOLT_API std::shared_ptr<gpu::rasterization_pool> create_rasterization_pool();

	VOLT_API std::shared_ptr<gpu::compute_pool> create_compute_pool();

	VOLT_API std::shared_ptr<gpu::copy_pool> create_copy_pool();

	VOLT_API const std::shared_ptr<gpu::adapter> &get_adapter();

protected:
	std::shared_ptr<gpu::adapter> adapter;

	VOLT_API device(std::shared_ptr<gpu::adapter> &&adapter);
};

}
