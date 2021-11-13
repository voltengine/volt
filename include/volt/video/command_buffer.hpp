#pragma once

#include "../macros.hpp"

namespace volt::video {

class command_buffer {
public:
#ifndef NDEBUG
	bool _requires_graphics()
	bool _requires_compute()
	bool _requires_transfer()
#endif

	// Common
	void begin()
	void end()

	void barrier() // all-resource barrier, image barrier, buffer barrier
	// Graphics
	void begin_pass() // framebuffer, renderArea, clear_color, + update in pass: viewport, scissor = renderArea // vkCmdBeginRenderPass, vkCmdSetViewport, vkCmdSetScissor
	void graphics_pipeline() // vkCmdBindPipeline
	void descriptors_sets() // vkCmdBindDescriptorSets
	void vertex_buffer() // vkCmdBindVertexBuffers
	void index_buffer() // vkCmdBindIndexBuffer
	void draw() // vkCmdDraw, vkCmdDrawIndexed
	void end_pass() // vkCmdEndRenderPass
	// Compute
	void compute_pipeline() // vkCmdBindPipeline
	void descriptors_sets() // vkCmdBindDescriptorSets
	void dispatch() // vkCmdDispatch

	// Transfer
	void copy_image_to_image() // vkCmdCopyImage
	void copy_image_to_buffer() // vkCmdCopyImageToBuffer
	void copy_buffer_to_image() // vkCmdCopyBufferToImage

protected:
	VOLT_API command_buffer() = default;
};

}
