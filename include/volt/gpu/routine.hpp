#pragma once

#include "../macros.hpp"

#include <memory>

#include "enums.hpp"

namespace volt::gpu {

template<command_type T>
class pool;

template<command_type T, bool Sub = false>
class routine {
public:
	virtual void begin(bool disposable) = 0;

	// virtual void begin(bool disposable, render_pass, uint32_t subpass, framebuffer, bool occlusion_query, bool precise_query) = 0;

	virtual void end() = 0;

	virtual void execute_subroutine(const std::shared_ptr<gpu::routine<T, true>> &subroutine) = 0;

	const std::shared_ptr<gpu::pool<T>> &get_pool() {
		return pool;
	}

protected:
	std::shared_ptr<gpu::pool<T>> pool;

	routine(std::shared_ptr<gpu::pool<T>> &&pool)
			: pool(std::move(pool)) {}
};

template<command_type T>
using subroutine = routine<T, true>;

using graphics_routine = routine<command_type::graphics>;
using compute_routine = routine<command_type::compute>;
using copy_routine = routine<command_type::copy>;

using graphics_subroutine = subroutine<command_type::graphics>;
using compute_subroutine = subroutine<command_type::compute>;
using copy_subroutine = subroutine<command_type::copy>;

// TODO: Specialize graphics, compute, copy routines

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
	// //void graphics_pipeline() // vkCmdBindPipeline
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


