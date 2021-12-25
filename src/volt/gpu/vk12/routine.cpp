#include <volt/pch.hpp>
#include <volt/gpu/vk12/routine.hpp>

#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/pass.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

routine_impl::~routine_impl() {
	vkDestroyFence(vk12_device.vk_device, finish_fence, nullptr);
	vkFreeCommandBuffers(vk12_device.vk_device, command_pool, 1, &command_buffer);
	vkDestroyCommandPool(vk12_device.vk_device, command_pool, nullptr);
}

// Used only in this one translation unit
template<typename Executor>
routine_impl routine_impl::_new(vk12::device &vk12_device) {
	routine_impl impl(vk12_device);
	vk12::adapter &adapter = *static_cast<vk12::adapter *>(vk12_device.adapter().get());

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

	if constexpr (std::is_same_v<Executor, gpu::universal_executor>) {
		impl.queue = vk12_device.universal_queue;
		pool_info.queueFamilyIndex = adapter.universal_family;
	} else if constexpr (std::is_same_v<Executor, gpu::compute_executor>) {
		impl.queue = vk12_device.compute_queue;
		pool_info.queueFamilyIndex = adapter.compute_family;
	} else {
		impl.queue = vk12_device.copy_queue;
		pool_info.queueFamilyIndex = adapter.copy_family;
	}
	
	VOLT_VK12_CHECK(vkCreateCommandPool(vk12_device.vk_device, &pool_info, nullptr, &impl.command_pool),
			"Failed to create command pool.")

	VkCommandBufferAllocateInfo command_buffer_info{};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.commandPool = impl.command_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	VOLT_VK12_CHECK(vkAllocateCommandBuffers(vk12_device.vk_device, &command_buffer_info, &impl.command_buffer),
			"Failed to allocate command buffers.")

	// VkSemaphoreCreateInfo semaphore_info{};
	// semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Signaled because routine has finished

	// VOLT_VK12_CHECK(vkCreateSemaphore(vk12_device.vk_device, &semaphore_info, nullptr,
	// 		&finish_semaphore), "Failed to create semaphore.")
	VOLT_VK12_CHECK(vkCreateFence(vk12_device.vk_device, &fence_info, nullptr,
			&impl.finish_fence), "Failed to create semaphore.")

	return impl;
}

void routine_impl::copy_buffer(
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

void routine_impl::copy_texture(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst) {
	auto &vk12_src = *static_cast<vk12::texture *>(src.get());
	auto &vk12_dst = *static_cast<vk12::texture *>(dst.get());

	vk12_src.barrier(command_buffer, vk12::texture::state::copy_src);
	vk12_dst.barrier(command_buffer, vk12::texture::state::copy_dst);

	VkImageAspectFlags aspect_mask = vk12_src.aspect_mask & vk12_dst.aspect_mask;

	uint32_t levels = math::min(src->levels(), dst->levels());	
	std::vector<VkImageCopy> regions(levels);

	for (uint32_t i = 0; i < levels; i++) {
		VkImageCopy &region = regions[i];
		region.srcSubresource = {
			.aspectMask = aspect_mask,
			.mipLevel = i,
			.baseArrayLayer = 0,
			.layerCount = 1 // Handled by extent
		};

		region.dstSubresource = {
			.aspectMask = aspect_mask,
			.mipLevel = i,
			.baseArrayLayer = 0,
			.layerCount = 1 // Handled by extent
		};

		uvec3 max_size = min(src->size() / pow(2, i),
				dst->size() / pow(2, i));
		region.extent = { max_size.x, max_size.y, max_size.z };
	}

	vkCmdCopyImage(command_buffer,
			vk12_src.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			vk12_dst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			regions.size(), regions.data());
}

void routine_impl::copy_texture_level(
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

	if (math::all(size == math::uvec3::zero)) {
		uvec3 max_size = min(src->size() / pow(2, src_level),
				dst->size() / pow(2, dst_level));
		region.extent = { max_size.x, max_size.y, max_size.z };
	} else
		region.extent = { size.x, size.y, size.z };

	vkCmdCopyImage(command_buffer,
			vk12_src.image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			vk12_dst.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &region);
}

void routine_impl::compute_pass(const compute_pass_info &info,
		const std::function<void(gpu::compute_pass &)> &callback) {
	// Init pass
	if (!vk12_device.compute_pass_cache.contains(info))
		vk12_device.compute_pass_cache.emplace(info, vk12::compute_pass(*this, info));
	vk12::compute_pass &pass = vk12_device.compute_pass_cache.at(info);

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pass.pipeline);
	callback(pass);
	pass.reset(); // Clear cached bindings
}

void routine_impl::rasterization_pass(const rasterization_pass_info &info,
		const std::function<void(gpu::rasterization_pass &)> &callback) {
	// Init pass
	if (!vk12_device.rasterization_pass_cache.contains(info))
		vk12_device.rasterization_pass_cache.emplace(info, vk12::rasterization_pass(*this, info));
	vk12::rasterization_pass &pass = vk12_device.rasterization_pass_cache.at(info);

	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pass.pipeline);
	callback(pass);
	pass.reset(); // Clear cached bindings
	
}

std::vector<VkDescriptorSet> routine_impl::_allocate_descriptor_sets(const std::vector<VkDescriptorSetLayout> &layouts,
		uint32_t constant_buffer_count, uint32_t sampled_texture_count, uint32_t storage_buffer_count, uint32_t storage_texture_count) {
	sets_allocated_from_current_pool             += layouts.size();
	constant_buffers_allocated_from_current_pool += constant_buffer_count;
	sampled_textures_allocated_from_current_pool += sampled_texture_count;
	storage_buffers_allocated_from_current_pool  += storage_buffer_count;
	storage_textures_allocated_from_current_pool += storage_texture_count;

	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = descriptor_pools[current_pool];
	allocate_info.descriptorSetCount = layouts.size();
	allocate_info.pSetLayouts = layouts.data();

	std::vector<VkDescriptorSet> descriptor_sets(layouts.size());
	VkResult result = vkAllocateDescriptorSets(vk12_device.vk_device, &allocate_info, descriptor_sets.data());

	if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
		if (++current_pool == descriptor_pools.size())
			_add_descriptor_pool();
		
		allocate_info.descriptorPool = descriptor_pools[current_pool];
		result = vkAllocateDescriptorSets(vk12_device.vk_device, &allocate_info, descriptor_sets.data());
	}

	VOLT_VK12_DEBUG_CHECK(result, "Failed to allocate descriptor sets.")

	return descriptor_sets;
}

routine_impl::routine_impl(vk12::device &vk12_device) : vk12_device(vk12_device) {
	_add_descriptor_pool();
}

void routine_impl::_add_descriptor_pool() {
	constexpr uint32_t sets_per_pool = 1024;
	VkDescriptorPoolSize pool_sizes[4];

	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = sets_per_pool * (static_cast<float>(constant_buffers_allocated_from_current_pool) / sets_allocated_from_current_pool);

	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[1].descriptorCount = sets_per_pool * (static_cast<float>(sampled_textures_allocated_from_current_pool) / sets_allocated_from_current_pool);

	pool_sizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	pool_sizes[2].descriptorCount = sets_per_pool * (static_cast<float>(storage_buffers_allocated_from_current_pool) / sets_allocated_from_current_pool);

	pool_sizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	pool_sizes[3].descriptorCount = sets_per_pool * (static_cast<float>(storage_textures_allocated_from_current_pool) / sets_allocated_from_current_pool);

	VkDescriptorPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = 4;
	pool_info.pPoolSizes = pool_sizes;
	pool_info.maxSets = sets_per_pool;

	VkDescriptorPool pool;
	VOLT_VK12_CHECK(vkCreateDescriptorPool(vk12_device.vk_device, &pool_info,
			nullptr, &pool), "Failed to create descriptor pool.")
	descriptor_pools.push_back(pool);

	sets_allocated_from_current_pool             = 1;
	constant_buffers_allocated_from_current_pool = 1;
	sampled_textures_allocated_from_current_pool = 1;
	storage_buffers_allocated_from_current_pool  = 1;
	storage_textures_allocated_from_current_pool = 1;
}

template<typename Executor>
routine<Executor>::routine(std::shared_ptr<gpu::device> &&device)
		: gpu::_internal::routine<Executor>(std::move(device)),
		impl(routine_impl::_new<Executor>(*static_cast<vk12::device *>(this->_device.get()))) {}

template<typename Executor>
void routine<Executor>::execute(const std::function<void(Executor &)> &callback) {
	VOLT_VK12_DEBUG_CHECK(vkResetCommandPool(impl.vk12_device.vk_device, impl.command_pool, 0),
			"Failed to reset command pool.")

	// Reset descriptor pools
	for (VkDescriptorPool pool : impl.descriptor_pools)
		vkResetDescriptorPool(impl.vk12_device.vk_device, pool, 0);
	impl.current_pool = 0;

	// Begin command buffer
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VOLT_VK12_DEBUG_CHECK(vkBeginCommandBuffer(impl.command_buffer, &begin_info),
			"Failed to begin command buffer.")
	
	// Execute user callback
	Executor executor = Executor::_new(impl);
	callback(executor);

	// End command buffer
	VOLT_VK12_DEBUG_CHECK(vkEndCommandBuffer(impl.command_buffer),
			"Failed to end command buffer.")

	// Todo finalize render pass definitions

	// VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// Submit
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

template class vk12::routine<universal_executor>;
template class vk12::routine<compute_executor>;
template class vk12::routine<copy_executor>;

}
