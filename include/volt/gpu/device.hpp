#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "adapter.hpp"
#include "buffer.hpp"
#include "enums.hpp"
#include "fence.hpp"
#include "queue.hpp"
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
			gpu::command_types sync_queues,
			gpu::buffer_features features,
			size_t size) = 0;

	virtual std::shared_ptr<gpu::fence> create_fence(uint64_t initial_value = 0) = 0;

	virtual std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) = 0;

	virtual std::shared_ptr<gpu::texture> create_2d_texture(
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			math::uvec2 size, uint32_t levels,
			gpu::texture_format format) = 0;

	virtual std::shared_ptr<gpu::texture> create_texture(
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::texture_features features,
			math::uvec3 size, uint32_t levels,
			gpu::texture_format format) = 0;
	
	// virtual std::shared_ptr<gpu::pipeline> create_pipeline() = 0;

	VOLT_API std::shared_ptr<gpu::rasterization_queue> get_rasterization_queue();

	VOLT_API std::shared_ptr<gpu::compute_queue> get_compute_queue();

	VOLT_API std::shared_ptr<gpu::copy_queue> get_copy_queue();

	VOLT_API const std::shared_ptr<gpu::adapter> &get_adapter();

protected:
	std::weak_ptr<gpu::rasterization_queue> rasterization_queue;
	std::weak_ptr<gpu::compute_queue> compute_queue;
	std::weak_ptr<gpu::copy_queue> copy_queue;
	std::shared_ptr<gpu::adapter> adapter;

	VOLT_API device(std::shared_ptr<gpu::adapter> &&adapter);

	virtual std::shared_ptr<gpu::rasterization_queue> new_rasterization_queue() = 0;

	virtual std::shared_ptr<gpu::compute_queue> new_compute_queue() = 0;

	virtual std::shared_ptr<gpu::copy_queue> new_copy_queue() = 0;
};

}
