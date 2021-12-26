#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "adapter.hpp"
#include "buffer.hpp"
#include "enums.hpp"
#include "routine.hpp"
#include "sampler.hpp"
#include "shader.hpp"
#include "swapchain.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device : public std::enable_shared_from_this<device> {
public:
	virtual ~device() = default;

	virtual void wait() = 0;

	virtual std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) = 0;

	virtual std::shared_ptr<gpu::universal_routine> create_universal_routine() = 0;

	virtual std::shared_ptr<gpu::compute_routine> create_compute_routine() = 0;

	virtual std::shared_ptr<gpu::copy_routine> create_copy_routine() = 0;

	virtual std::shared_ptr<gpu::buffer> create_buffer(
			gpu::memory_type memory_type,
			gpu::buffer_features features,
			size_t size) = 0;

	virtual std::shared_ptr<gpu::texture> create_1d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, uint32_t size, uint32_t layers = 1) = 0;

	virtual std::shared_ptr<gpu::texture> create_2d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec2 size, uint32_t layers = 1) = 0;

	virtual std::shared_ptr<gpu::texture> create_3d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec3 size) = 0;

	virtual std::shared_ptr<gpu::texture> create_cube_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec2 size, uint32_t layers = 1) = 0;
	
	virtual std::shared_ptr<gpu::sampler> create_sampler(
			texture_filter filter = texture_filter::nearest, bool blur = false, float anisotropy = 0) = 0;

	virtual std::shared_ptr<gpu::shader> create_shader(const std::vector<uint8_t> &bytecode) = 0;

	VOLT_API const std::shared_ptr<gpu::adapter> &adapter();

protected:
	std::shared_ptr<gpu::adapter> _adapter;
	// Command Buffer Pool + Command Buffer for copying
	// Auto-Sync trackers

	VOLT_API device(std::shared_ptr<gpu::adapter> &&adapter);
};

}
