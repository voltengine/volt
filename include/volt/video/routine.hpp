#pragma once

#include "../macros.hpp"

namespace volt::video {

class routine {
public:
// #ifndef NDEBUG
// 	video::queue::type get_queue_type();
// #endif

	// Common
	// void reset();
	// void begin();
	// void end();

	// //void barrier() // all-resource barrier, image barrier, buffer barrier
	// // Graphics
	// //void begin_pass() // framebuffer, renderArea, clear_color, + update in pass: viewport, scissor = renderArea // vkCmdBeginRenderPass, vkCmdSetViewport, vkCmdSetScissor
	// //void graphics_pipeline() // vkCmdBindPipeline
	// //void descriptors_sets() // vkCmdBindDescriptorSets
	// void vertex_buffer(const std::shared_ptr<video::buffer> &buffer) // vkCmdBindVertexBuffers
	// void index_buffer(const std::shared_ptr<video::buffer> &buffer) // vkCmdBindIndexBuffer
	// //void draw() // vkCmdDraw, vkCmdDrawIndexed
	// //void end_pass() // vkCmdEndRenderPass
	// void blit_texture(const std::shared_ptr<video::texture> &src, const std::shared_ptr<video::texture> dst, bool filter)
	// // Compute
	// //void compute_pipeline() // vkCmdBindPipeline
	//void descriptors_sets() // vkCmdBindDescriptorSets
	//void dispatch() // vkCmdDispatch

	// Copy
	//void copy_image() // vkCmdCopyImage
	//void copy_image_to_buffer() // vkCmdCopyImageToBuffer
	//void copy_buffer_to_image() // vkCmdCopyBufferToImage
	// void copy_buffer // vkCmdCopyBuffer

protected:
// #ifndef NDEBUG
// 	video::queue::type queue_type;
// #endif

	VOLT_API routine() = default;
};

}
