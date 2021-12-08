#include <volt/pch.hpp>
#include <volt/gpu/vk12/routine.hpp>

#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>

namespace volt::gpu::vk12 {

using namespace math;

template<typename Executor>
routine_impl<Executor>::routine_impl(vk12::device &vk12_device) : vk12_device(vk12_device) {
	vk12::adapter &adapter = *static_cast<vk12::adapter *>(vk12_device.adapter().get());

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	
	if constexpr (std::is_same_v<Executor, gpu::graphics_executor>) {
		queue = vk12_device.graphics_queue;
		pool_info.queueFamilyIndex = adapter.graphics_family;
	} else if constexpr (std::is_same_v<Executor, gpu::compute_executor>) {
		queue = vk12_device.compute_queue;
		pool_info.queueFamilyIndex = adapter.compute_family;
	} else {
		queue = vk12_device.copy_queue;
		pool_info.queueFamilyIndex = adapter.copy_family;
	}
	
	VOLT_VK12_CHECK(vkCreateCommandPool(vk12_device.vk_device, &pool_info, nullptr, &command_pool),
			"Failed to create command pool.")

	VkCommandBufferAllocateInfo command_buffer_info{};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.commandPool = command_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	VOLT_VK12_CHECK(vkAllocateCommandBuffers(vk12_device.vk_device, &command_buffer_info, &command_buffer),
			"Failed to allocate command buffers.")

	// VkSemaphoreCreateInfo semaphore_info{};
	// semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Signaled because routine has finished

	// VOLT_VK12_CHECK(vkCreateSemaphore(vk12_device.vk_device, &semaphore_info, nullptr,
	// 		&finish_semaphore), "Failed to create semaphore.")
	VOLT_VK12_CHECK(vkCreateFence(vk12_device.vk_device, &fence_info, nullptr,
			&finish_fence), "Failed to create semaphore.")
}

template<typename Executor>
routine_impl<Executor>::~routine_impl() {
	vkDestroyFence(vk12_device.vk_device, finish_fence, nullptr);
	vkFreeCommandBuffers(vk12_device.vk_device, command_pool, 1, &command_buffer);
	vkDestroyCommandPool(vk12_device.vk_device, command_pool, nullptr);
}

template<typename Executor>
void routine_impl<Executor>::copy_buffer(
		const std::shared_ptr<gpu::buffer> &src,
		const std::shared_ptr<gpu::buffer> &dst,
		size_t src_offset,
		size_t dst_offset,
		size_t size) {
	auto &vk12_src = *static_cast<vk12::buffer *>(src.get());
	auto &vk12_dst = *static_cast<vk12::buffer *>(dst.get());

	// TODO: Make barriers support regions
	vk12_src.barrier(command_buffer, vk12::buffer::state::copy_src);
	vk12_dst.barrier(command_buffer, vk12::buffer::state::copy_dst);

	VkBufferCopy region{
		.srcOffset = 0,
		.dstOffset = 0,
		.size = math::min(src->size(), dst->size())
	};

	vkCmdCopyBuffer(command_buffer, vk12_src.vk_buffer, vk12_dst.vk_buffer, 1, &region);
}

template<typename Executor>
void routine_impl<Executor>::copy_texture_level(
		const std::shared_ptr<gpu::texture> &src,
		const std::shared_ptr<gpu::texture> &dst,
		uint32_t src_level,
		uint32_t dst_level,
		math::uvec3 src_offset,
		math::uvec3 dst_offset,
		math::uvec3 size) {
	auto &vk12_src = *static_cast<vk12::texture *>(src.get());
	auto &vk12_dst = *static_cast<vk12::texture *>(dst.get());

	// TODO: Make barriers support regions
	vk12_src.barrier(command_buffer, vk12::texture::state::copy_src);
	vk12_dst.barrier(command_buffer, vk12::texture::state::copy_dst);

	VkImageAspectFlags aspect_mask = vk12_src.aspect_mask & vk12_dst.aspect_mask;
	uvec3 max_size = min(src->size() / pow(2, src_level),
			dst->size() / pow(2, dst_level));

	VkImageCopy region{};
	region.srcSubresource = {
		.aspectMask = aspect_mask,
		.mipLevel = src_level,
		.baseArrayLayer = 0,
		.layerCount = 1 // Handled by extent
	};
	region.srcOffset = {
		static_cast<int32_t>(src_offset.x),
		static_cast<int32_t>(src_offset.y),
		static_cast<int32_t>(src_offset.z)
	};

	region.dstSubresource = {
		.aspectMask = aspect_mask,
		.mipLevel = dst_level,
		.baseArrayLayer = 0,
		.layerCount = 1 // Handled by extent
	};
	region.dstOffset = {
		static_cast<int32_t>(dst_offset.x),
		static_cast<int32_t>(dst_offset.y),
		static_cast<int32_t>(dst_offset.z)
	};

	if (math::all(size == math::uvec3::zero))
		region.extent = { max_size.x, max_size.y, max_size.z };
	else
		region.extent = { size.x, size.y, size.z };

	vkCmdCopyImage(command_buffer,
			vk12_src.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			vk12_dst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &region);
}

template<typename Executor>
void routine_impl<Executor>::compute_pass(const compute_pass_info &info,
		const std::function<void(gpu::compute_pass &)> &callback) {

}

template<typename Executor>
void routine_impl<Executor>::rasterization_pass(const rasterization_pass_info &info,
		const std::function<void(gpu::rasterization_pass &)> &callback) {

}

template<typename Executor>
routine<Executor>::routine(std::shared_ptr<gpu::device> &&device)
		: gpu::_internal::routine<Executor>(std::move(device)),
		impl(*static_cast<vk12::device *>(this->_device.get())) {}

template<typename Executor>
routine<Executor>::~routine() {
	wait();
}

template<typename Executor>
void routine<Executor>::execute(const std::function<void(Executor &)> &callback) {
	VOLT_VK12_DEBUG_CHECK(vkResetCommandPool(impl.vk12_device.vk_device, impl.command_pool, 0),
			"Failed to reset command pool.")

	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VOLT_VK12_DEBUG_CHECK(vkBeginCommandBuffer(impl.command_buffer, &begin_info),
			"Failed to begin command buffer.")
	
	Executor executor = Executor::_new(impl);
	callback(executor);

	VOLT_VK12_DEBUG_CHECK(vkEndCommandBuffer(impl.command_buffer),
			"Failed to end command buffer.")

	// Todo finalize render pass definitions

	// VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	// submit_info.waitSemaphoreCount = 1;
	// submit_info.pWaitSemaphores = &wait_semaphore;
	// submit_info.pWaitDstStageMask = &wait_stage;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &impl.command_buffer;
	// submit_info.signalSemaphoreCount = 1;
	// submit_info.pSignalSemaphores = &finish_semaphore;

	VOLT_VK12_DEBUG_CHECK(vkResetFences(impl.vk12_device.vk_device, 1, &impl.finish_fence),
			"Failed to reset fences")
	VOLT_VK12_DEBUG_CHECK(vkQueueSubmit(impl.queue, 1, &submit_info, impl.finish_fence),
			"Failed to submit to queue.")
}

template<typename Executor>
bool routine<Executor>::finished() {
	VkResult result = vkGetFenceStatus(impl.vk12_device.vk_device, impl.finish_fence);
	return result == VK_SUCCESS;
}

template<typename Executor>
void routine<Executor>::wait() {
	vkWaitForFences(impl.vk12_device.vk_device, 1, &impl.finish_fence,
			VK_FALSE, std::numeric_limits<uint64_t>::max());
}

template class vk12::routine<graphics_executor>;
template class vk12::routine<compute_executor>;
template class vk12::routine<copy_executor>;

}
