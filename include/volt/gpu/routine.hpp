#pragma once

#include "../macros.hpp"

#include <memory>

#include "../math/math.hpp"
#include "enums.hpp"

namespace volt::gpu {

template<command_types T>
class pool;

template<command_types T>
class routine {
public:
	virtual ~routine() = default;

	virtual void begin() = 0;

	virtual void end() = 0;
// TODO make pure
	// virtual void viewport(math::uvec2 width, math::uvec2 height, math::uvec2 depth) = 0;

	// virtual void scissor(math::uvec2 width, math::uvec2 height) = 0;

	// virtual void clear_color(math::fvec3 color) = 0;

	// TODO: Might crash if vector is deleted after the call? To be tested
	// virtual void vertex_buffers(uint32_t offset,
	// 		const std::vector<std::pair<const std::shared_ptr<
	// 		gpu::buffer> &, uint32_t>> &buffers_with_offsets) = 0;

	// virtual void index_buffer(const std::shared_ptr<gpu::buffer> &buffer) = 0;

	// virtual void draw(uint32_t index_count, uint32_t instance_count,
	// 		uint32_t index_offset = 0, uint32_t instance_offset = 0, uint32_t vertex_offset = 0) = 0;

	const std::shared_ptr<gpu::pool<T>> &get_pool() {
		return pool;
	}

protected:
	std::shared_ptr<gpu::pool<T>> pool;

	routine(std::shared_ptr<gpu::pool<T>> &&pool)
			: pool(std::move(pool)) {}
};

using rasterization_routine = routine<command_type::rasterization>;
using compute_routine = routine<command_type::compute>;
using copy_routine = routine<command_type::copy>;

}

// #pragma once

// #include "../macros.hpp"

// namespace volt::video {

// #ifndef NDEBUG
// 	gpu::queue_type get_queue_type();
// #endif

	// Common
	// void begin();
	// void end();

	// //void barrier() // all-resource barrier, image barrier, buffer barrier
	// // Graphics
	// //void begin_pass() // framebuffer, renderArea, clear_color, + update in pass: viewport, scissor = renderArea // vkCmdBeginRenderPass, vkCmdSetViewport, vkCmdSetScissor
	// //void rasterization_pipeline() // vkCmdBindPipeline
	// //void descriptors_sets() // vkCmdBindDescriptorSets
	// void vertex_buffer(const std::shared_ptr<gpu::buffer> &buffer) // vkCmdBindVertexBuffers
	// void index_buffer(const std::shared_ptr<gpu::buffer> &buffer) // vkCmdBindIndexBuffer
	// //void draw() // vkCmdDraw, vkCmdDrawIndexed
	// //void end_pass() // vkCmdEndRenderPass
	// void blit_texture(const std::shared_ptr<gpu::texture> &src, const std::shared_ptr<gpu::texture> dst, bool filter)
	// // Compute
	// //void compute_pipeline() // vkCmdBindPipeline
	//void descriptors_sets() // vkCmdBindDescriptorSets
	//void dispatch() // vkCmdDispatch

	// Copy
	//void copy_image() // vkCmdCopyImage
	//void copy_image_to_buffer() // vkCmdCopyImageToBuffer
	//void copy_buffer_to_image() // vkCmdCopyBufferToImage
	// void copy_buffer // vkCmdCopyBuffer


