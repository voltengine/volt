#include <volt/gpu/vk12/pass.hpp>

#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

pass_context::pass_context(vk12::routine_impl &impl) : impl(impl) {}

void pass_context::draw(const draw_info &info) {
	vk12::rasterization_pipeline_key key{ // TODO: Use bitset
		.render_pass = impl.current_render_pass,
		.vertex_shader = info.vertex_shader,
		.hull_shader = info.hull_shader,
		.domain_shader = info.domain_shader,
		.geometry_shader = info.geometry_shader,
		.pixel_shader = info.pixel_shader,
		.instance_inputs = info.instance_inputs,
		.polygon_mode = info.polygon_mode,
		.primitive_mode = info.primitive_mode,
		.culling = info.culling,
		.depth_test = info.depth_test,
		.depth_write = info.depth_write,
		.stencil_test = info.stencil_test,
		.stencil_write = info.stencil_write,
		.blending = info.blending
	};

	vk12::pipeline &pipeline = impl.vk12_device.jit.rasterization_pipeline(std::move(key));
	vkCmdBindPipeline(impl.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.vk_pipeline);
	impl._update_descriptor_sets(info, pipeline);

	// Dynamic state
	VkViewport vk_viewport{
		.x = info.viewport.width.x,
		.y = info.viewport.height.x,
		.width = info.viewport.width.y - info.viewport.width.x,
		.height = info.viewport.height.y - info.viewport.height.x,
		.minDepth = info.viewport.depth.x,
		.maxDepth = info.viewport.depth.y
	};

	VkRect2D vk_scissor{
		.offset = {
			static_cast<int32_t>(info.viewport.width.x + 0.5F),
			static_cast<int32_t>(info.viewport.height.x + 0.5F)
		},
		.extent = {
			static_cast<uint32_t>(info.viewport.width.y + 0.5F),
			static_cast<uint32_t>(info.viewport.height.y + 0.5F)
		}
	};

	vkCmdSetViewport(impl.command_buffer, 0, 1, &vk_viewport);
	vkCmdSetScissor(impl.command_buffer, 0, 1, &vk_scissor);
	vkCmdSetLineWidth(impl.command_buffer, info.line_width);

	// Buffers
	if (info.index_buffer) {
		auto &vk12_buffer = *static_cast<vk12::buffer *>(info.index_buffer);
		vk12_buffer.barrier(impl, vk12::buffer::state::index_input);

		vkCmdBindIndexBuffer(impl.command_buffer, vk12_buffer.vk_buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	VkDeviceSize buffer_offset = 0;

	if (info.vertex_buffer) {
		auto &vk12_buffer = *static_cast<vk12::buffer *>(info.vertex_buffer);
		vk12_buffer.barrier(impl, vk12::buffer::state::vertex_input);

		vkCmdBindVertexBuffers(impl.command_buffer, 0, 1, &vk12_buffer.vk_buffer, &buffer_offset);
	} else {
		VkBuffer buffer = VK_NULL_HANDLE;
		vkCmdBindVertexBuffers(impl.command_buffer, 0, 1, &buffer, &buffer_offset);
	}

	if (info.instance_buffer) {
		auto &vk12_buffer = *static_cast<vk12::buffer *>(info.instance_buffer);
		vk12_buffer.barrier(impl, vk12::buffer::state::vertex_input);

		vkCmdBindVertexBuffers(impl.command_buffer, 1, 1, &vk12_buffer.vk_buffer, &buffer_offset);
	} else {
		VkBuffer buffer = VK_NULL_HANDLE;
		vkCmdBindVertexBuffers(impl.command_buffer, 1, 1, &buffer, &buffer_offset);
	}

	if (info.index_buffer)
		vkCmdDrawIndexed(impl.command_buffer, info.draw_count, info.instance_count, 0, 0, 0);
	else
		vkCmdDraw(impl.command_buffer, info.draw_count, info.instance_count, 0, 0);
}

async_pass_context::async_pass_context(vk12::routine_impl &impl)
		: gpu::async_pass_context(impl.vk12_device.thread_pool), impl(impl) {}

void async_pass_context::draw(uint32_t thread_index, const draw_info &info) {
	// TODO
}

}
