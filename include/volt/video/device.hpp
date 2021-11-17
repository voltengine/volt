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
#include "surface.hpp"
#include "texture.hpp"

namespace volt::video {

class device : public std::enable_shared_from_this<device> {
public:
	virtual void wait() = 0;

	virtual std::shared_ptr<video::buffer> create_buffer(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::buffer_features features,
			size_t size) = 0;

	virtual std::shared_ptr<video::fence> create_fence(uint64_t initial_value = 0) = 0;

	virtual std::shared_ptr<video::surface> create_surface(std::shared_ptr<os::window> window) = 0;

	virtual std::shared_ptr<video::texture> create_texture(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			size_t size, uint32_t levels, uint32_t layers,
			video::texture_format format) = 0;

	virtual std::shared_ptr<video::texture> create_texture(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			video::texture_format format) = 0;

	virtual std::shared_ptr<video::texture> create_texture(
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::texture_features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			video::texture_format format) = 0;
	
	// virtual std::shared_ptr<video::pipeline> create_pipeline() = 0;

	VOLT_API std::shared_ptr<video::graphics_queue> access_graphics_queue();

	VOLT_API std::shared_ptr<video::compute_queue> access_compute_queue();

	VOLT_API std::shared_ptr<video::copy_queue> access_copy_queue();

	VOLT_API const std::shared_ptr<video::adapter> &get_adapter();

protected:
	std::weak_ptr<video::graphics_queue> graphics_queue;
	std::weak_ptr<video::compute_queue> compute_queue;
	std::weak_ptr<video::copy_queue> copy_queue;
	std::shared_ptr<video::adapter> adapter;

	VOLT_API device(std::shared_ptr<video::adapter> &&adapter);

	virtual std::shared_ptr<video::graphics_queue> new_graphics_queue() = 0;

	virtual std::shared_ptr<video::compute_queue> new_compute_queue() = 0;

	virtual std::shared_ptr<video::copy_queue> new_copy_queue() = 0;
};

}
