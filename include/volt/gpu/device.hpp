#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "adapter.hpp"
#include "buffer.hpp"
#include "graph.hpp"
#include "sampler.hpp"
#include "shader.hpp"
#include "swapchain.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device : public std::enable_shared_from_this<device> {
public:
	virtual ~device() = default;

	virtual void wait() = 0;

	virtual void graph(const std::function<void(graph &)> &callback) = 0;

	virtual std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) = 0;

	virtual std::shared_ptr<gpu::buffer> create_buffer(
			gpu::buffer_features features,
			size_t size) = 0;

	virtual std::shared_ptr<gpu::texture> create_1d_texture(
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, uint32_t size, uint32_t layers = 1) = 0;

	virtual std::shared_ptr<gpu::texture> create_2d_texture(
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec2 size, uint32_t layers = 1) = 0;

	virtual std::shared_ptr<gpu::texture> create_3d_texture(
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec3 size) = 0;

	virtual std::shared_ptr<gpu::texture> create_cube_texture(
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec2 size, uint32_t layers = 1) = 0;
	
	virtual std::shared_ptr<gpu::sampler> create_sampler(
			sampler_filter filter = sampler_filter::nearest, bool blur = false, float anisotropy = 0) = 0;

	virtual std::shared_ptr<gpu::shader> create_shader(const std::vector<uint8_t> &bytecode) = 0;

	VOLT_API const std::shared_ptr<gpu::adapter> &adapter() const;

protected:
	std::shared_ptr<gpu::adapter> _adapter;

	VOLT_API device(std::shared_ptr<gpu::adapter> &&adapter);
};

}
