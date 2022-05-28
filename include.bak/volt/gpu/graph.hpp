#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "../math/math.hpp"
#include "../util/util.hpp"
#include "pass.hpp"

namespace volt::gpu {

struct copy_info {
	struct buffer {
		util::smart_ptr_view<gpu::buffer> src;
		util::smart_ptr_view<gpu::buffer> dst;

		uint32_t offset = 0;
		uint32_t size = 0;
	};

	struct texture {
		struct level {
			util::smart_ptr_view<gpu::texture> texture;
			uint32_t level;
		};

		level src;
		level dst;

		math::uvec3 offset = 0;
		math::uvec3 size = 0;
	};

	std::vector<buffer> buffers;
	std::vector<texture> textures;
	bool allow_async = false;
};

struct dispatch_info {
	std::vector<constant_buffer_binding> constant_buffers;
	std::vector<sampled_texture_binding> sampled_textures;
	std::vector<storage_buffer_binding> storage_buffers;
	std::vector<storage_texture_binding> storage_textures;
	
	util::smart_ptr_view<gpu::shader> compute_shader;
	math::uvec3 group_count;
	bool allow_async = false;
};

class graph {
public:
	virtual ~graph() = default;

	virtual void copy(copy_info &copy_info) = 0;

	virtual void dispatch(const dispatch_info &info) = 0;

	virtual void pass(const pass_info &info,
			const std::function<void(gpu::pass &)> &callback) = 0;

	virtual void async_pass(const pass_info &info,
			const std::function<void(gpu::async_pass &)> &callback) = 0;

protected:
	graph() = default;
};

}
