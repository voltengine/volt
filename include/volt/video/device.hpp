#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "adapter.hpp"
#include "buffer.hpp"
#include "queue.hpp"
#include "resource.hpp"
#include "shader.hpp"
#include "surface.hpp"
#include "texture.hpp"
#include "video.hpp"

namespace volt::video {

class device : public std::enable_shared_from_this<device> {
public:
	virtual void wait() = 0;

	virtual video::queue &get_graphics_queue() = 0;

	virtual video::queue &get_compute_queue() = 0;

	virtual video::queue &get_copy_queue() = 0;

	virtual std::shared_ptr<video::buffer> create_buffer(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::buffer::features features,
			size_t size) = 0;

	virtual std::shared_ptr<video::surface> create_surface(std::shared_ptr<os::window> window) = 0;

	virtual std::shared_ptr<video::texture> create_texture(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			size_t size, uint32_t levels, uint32_t layers,
			video::texture::format format) = 0;

	virtual std::shared_ptr<video::texture> create_texture(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			math::uvec2 size, uint32_t levels, uint32_t layers,
			video::texture::format format) = 0;

	virtual std::shared_ptr<video::texture> create_texture(
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::texture::features features,
			math::uvec3 size, uint32_t levels, uint32_t layers,
			video::texture::format format) = 0;

	virtual std::shared_ptr<video::shader> create_shader(const std::vector<uint8_t> &bytecode) = 0;
	
	// virtual std::shared_ptr<video::pipeline> create_pipeline() = 0;

	const std::shared_ptr<video::adapter> &get_adapter() {
		return adapter;
	}

protected:
	std::shared_ptr<video::adapter> adapter;

	device(std::shared_ptr<video::adapter> &&adapter)
			: adapter(std::move(adapter)) {}
};

}
