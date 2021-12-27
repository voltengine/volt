#include <volt/pch.hpp>
#include <volt/gpu/vk12/routine.hpp>

#include <volt/gpu/vk12/buffer.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/pass.hpp>
#include <volt/gpu/vk12/sampler.hpp>
#include <volt/gpu/vk12/shader.hpp>
#include <volt/gpu/vk12/texture.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

using namespace math;

routine_impl::routine_impl(vk12::device &vk12_device) : vk12_device(vk12_device) {}

void routine_impl::copy_buffer(
		const std::shared_ptr<gpu::buffer> &src,
		const std::shared_ptr<gpu::buffer> &dst,
		size_t src_offset,
		size_t dst_offset,
		size_t size) {
	auto &vk12_src = *static_cast<vk12::buffer *>(src.get());
	auto &vk12_dst = *static_cast<vk12::buffer *>(dst.get());

	// TODO: Make barriers support regions
	vk12_src.barrier(*this, vk12::buffer::state::copy_src);
	vk12_dst.barrier(*this, vk12::buffer::state::copy_dst);

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

	vk12_src.barrier(*this, vk12::texture::state::copy_src);
	vk12_dst.barrier(*this, vk12::texture::state::copy_dst);

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
	vk12_src.barrier(*this, vk12::texture::state::copy_src);
	vk12_dst.barrier(*this, vk12::texture::state::copy_dst);

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

void routine_impl::dispatch(const dispatch_info &info) {
	vk12::compute_pipeline_key key{
		.compute_shader = info.compute_shader
	};

	pipeline &pipeline = vk12_device.jit.compute_pipeline(std::move(key));
	_update_descriptor_sets(info, pipeline);
	vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.vk_pipeline);

	vkCmdDispatch(command_buffer, info.group_count.x, info.group_count.y, info.group_count.z);
}

void routine_impl::pass(const pass_info &info,
		const std::function<void(gpu::pass_context &)> &callback) {
	std::vector<VkFormat> color_attachment_formats;
	std::vector<gpu::attachment_initializer> color_attachment_initializers;
	std::vector<gpu::texture *> color_attachments;
	std::vector<VkClearValue> clear_values;

	color_attachment_formats.reserve(info.color_attachments.size());
	color_attachment_initializers.reserve(info.color_attachments.size());
	color_attachments.reserve(info.color_attachments.size());
	clear_values.reserve(info.color_attachments.size() + info.depth_stencil_attachment.has_value());

	for (const color_attachment_info &attachment : info.color_attachments) {
		VkClearValue clear_value{};
		clear_value.color = {
			.float32 = {
				attachment.clear_color.x,
				attachment.clear_color.y,
				attachment.clear_color.z,
				attachment.clear_color.w
			}
		};

		auto &vk12_texture = *static_cast<vk12::texture *>(attachment.texture);
		vk12_texture.barrier(*this, vk12::texture::state::color_attachment);

		color_attachment_formats.push_back(vk12_texture.vk_format);
		color_attachment_initializers.push_back(attachment.initializer);
		color_attachments.push_back(attachment.texture);
		clear_values.push_back(clear_value);
	}

	vk12::render_pass_key render_pass_key{
		.color_attachment_formats = std::move(color_attachment_formats),
		.color_attachment_initializers = std::move(color_attachment_initializers)
	};

	math::uvec3 size;
	gpu::texture *depth_stencil_attachment = nullptr;

	if (info.depth_stencil_attachment) {
		auto &value = info.depth_stencil_attachment.value();
		size = value.texture->size();

		VkClearValue clear_value{};
		clear_value.depthStencil = {
			.depth = value.clear_depth,
			.stencil = value.clear_stencil
		};
		clear_values.push_back(std::move(clear_value));

		depth_stencil_attachment = value.texture;
		auto &vk12_texture = *static_cast<vk12::texture *>(depth_stencil_attachment);
		vk12_texture.barrier(*this, vk12::texture::state::depth_stencil_attachment);

		render_pass_key.depth_stencil_attachment_format = vk12_texture.vk_format;
		render_pass_key.depth_stencil_attachment_initializer = value.initializer;
	} else
		size = info.color_attachments.front().texture->size();

	VkRenderPass render_pass = vk12_device.jit.render_pass(std::move(render_pass_key));

	vk12::framebuffer_key framebuffer_key{
		.render_pass = render_pass,
		.color_attachments = std::move(color_attachments),
		.depth_stencil_attachment = depth_stencil_attachment
	};

	VkFramebuffer framebuffer = vk12_device.jit.framebuffer(std::move(framebuffer_key));

	VkRenderPassBeginInfo render_pass_begin_info = {};
	render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	render_pass_begin_info.renderPass = render_pass;
	render_pass_begin_info.framebuffer = framebuffer;
	render_pass_begin_info.renderArea.offset = { 0, 0 };
	render_pass_begin_info.renderArea.extent = { size.x, size.y };
	render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
	render_pass_begin_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	current_render_pass = render_pass;

	vk12::pass_context context(*this);
	callback(context);

	current_render_pass = VK_NULL_HANDLE;
	vkCmdEndRenderPass(command_buffer);
}

void routine_impl::async_pass(const pass_info &info,
		const std::function<void(gpu::async_pass_context &)> &callback) {
	pass(info, [](gpu::pass_context &context) {
		// TODO: Create contextless execute_pass() function for pass() and async_pass()
		// Init parallel mode

		// gpu::async_pass_context async_context(*this);
		// callback(async_context)

		// Collect parallel work
	});
}

template<typename DescriptorInfo>
void routine_impl::_update_descriptor_sets(const DescriptorInfo &info, const pipeline &pipeline) {
	sets_allocated_from_current_pool             += pipeline.set_layouts.size();
	constant_buffers_allocated_from_current_pool += info.constant_buffers.size();
	sampled_textures_allocated_from_current_pool += info.sampled_textures.size();
	storage_buffers_allocated_from_current_pool  += info.storage_buffers.size();
	storage_textures_allocated_from_current_pool += info.storage_textures.size();

	VkDescriptorSetAllocateInfo allocate_info{};
	allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocate_info.descriptorPool = descriptor_pools[current_descriptor_pool];
	allocate_info.descriptorSetCount = pipeline.set_layouts.size();
	allocate_info.pSetLayouts = pipeline.set_layouts.data();

	std::vector<VkDescriptorSet> descriptor_sets(pipeline.set_layouts.size());
	VkResult result = vkAllocateDescriptorSets(vk12_device.vk_device, &allocate_info, descriptor_sets.data());

	if (result == VK_ERROR_OUT_OF_POOL_MEMORY) {
		if (++current_descriptor_pool == descriptor_pools.size())
			_add_descriptor_pool();
		
		allocate_info.descriptorPool = descriptor_pools[current_descriptor_pool];
		result = vkAllocateDescriptorSets(vk12_device.vk_device, &allocate_info, descriptor_sets.data());
	}

	VOLT_VK12_DEBUG_CHECK(result, "Failed to allocate descriptor sets.")

	std::vector<VkDescriptorBufferInfo> buffer_infos;
	std::vector<VkDescriptorImageInfo> image_infos;
	std::vector<VkWriteDescriptorSet> writes;

	buffer_infos.resize(info.constant_buffers.size() + info.storage_buffers.size());
	image_infos.resize(info.sampled_textures.size() + info.storage_textures.size());
	writes.reserve(buffer_infos.size() + image_infos.size());

	for (auto &item : info.constant_buffers) {
		// Translate slot name to binding point
		vk12::shader::binding_point binding_point = pipeline.binding_points.at(item.slot);

		auto *vk12_buffer = static_cast<vk12::buffer *>(item.buffer);

		if constexpr (std::is_same_v<DescriptorInfo, draw_info>)
			vk12_buffer->barrier(*this, vk12::buffer::state::rasterization_shared);
		else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>)
			vk12_buffer->barrier(*this, vk12::buffer::state::compute_shared);

		VkDescriptorBufferInfo &buffer_info = buffer_infos[writes.size()];
		buffer_info.buffer = vk12_buffer->vk_buffer;
		buffer_info.offset = 0;
		buffer_info.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		write.pBufferInfo = &buffer_info;

		writes.push_back(std::move(write));
	}

	for (auto &item : info.sampled_textures) {
		vk12::shader::binding_point binding_point = pipeline.binding_points.at(item.slot);

		auto *vk12_texture = static_cast<vk12::texture *>(item.texture);
		auto *vk12_sampler = static_cast<vk12::sampler *>(item.sampler);

		if constexpr (std::is_same_v<DescriptorInfo, draw_info>)
			vk12_texture->barrier(*this, vk12::texture::state::rasterization_shared);
		else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>)
			vk12_texture->barrier(*this, vk12::texture::state::compute_shared);

		VkDescriptorImageInfo &image_info = image_infos[writes.size()];
		image_info.sampler = vk12_sampler->vk_sampler;
		image_info.imageView = vk12_texture->image_view;
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.pImageInfo = &image_info;	

		writes.push_back(std::move(write));	
	}

	for (auto &item : info.storage_buffers) {
		vk12::shader::binding_point binding_point = pipeline.binding_points.at(item.slot);

		auto *vk12_buffer = static_cast<vk12::buffer *>(item.buffer);
		if (item.shared) {
			if constexpr (std::is_same_v<DescriptorInfo, draw_info>)
				vk12_buffer->barrier(*this, vk12::buffer::state::rasterization_shared);
			else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>)
				vk12_buffer->barrier(*this, vk12::buffer::state::compute_shared);
		} else {
			if constexpr (std::is_same_v<DescriptorInfo, draw_info>)
				vk12_buffer->barrier(*this, vk12::buffer::state::rasterization_unique);
			else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>)
				vk12_buffer->barrier(*this, vk12::buffer::state::compute_unique);
		}

		VkDescriptorBufferInfo &buffer_info = buffer_infos[writes.size()];
		buffer_info.buffer = vk12_buffer->vk_buffer;
		buffer_info.offset = 0;
		buffer_info.range = VK_WHOLE_SIZE;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		write.pBufferInfo = &buffer_info;	

		writes.push_back(std::move(write));	
	}

	for (auto &item : info.storage_textures) {
		vk12::shader::binding_point binding_point = pipeline.binding_points.at(item.slot);

		auto *vk12_texture = static_cast<vk12::texture *>(item.texture);
		if (item.shared) {
			if constexpr (std::is_same_v<DescriptorInfo, draw_info>)
				vk12_texture->barrier(*this, vk12::texture::state::rasterization_shared);
			else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>)
				vk12_texture->barrier(*this, vk12::texture::state::compute_shared);
		} else {
			if constexpr (std::is_same_v<DescriptorInfo, draw_info>)
				vk12_texture->barrier(*this, vk12::texture::state::rasterization_unique);
			else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>)
				vk12_texture->barrier(*this, vk12::texture::state::compute_unique);
		}

		VkDescriptorImageInfo &image_info = image_infos[writes.size()];
		image_info.sampler = VK_NULL_HANDLE;
		image_info.imageView = vk12_texture->image_view;
		image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write{};
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.dstSet = descriptor_sets[binding_point.set];
		write.dstBinding = binding_point.binding;
		write.dstArrayElement = 0;
		write.descriptorCount = 1;
		write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		write.pImageInfo = &image_info;

		writes.push_back(std::move(write));	
	}
	
	vkUpdateDescriptorSets(vk12_device.vk_device, static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);

	if constexpr (std::is_same_v<DescriptorInfo, draw_info>) {
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.pipeline_layout, 0,
				static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
	} else if constexpr (std::is_same_v<DescriptorInfo, dispatch_info>) {
		vkCmdBindDescriptorSets(command_buffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.pipeline_layout, 0,
				static_cast<uint32_t>(descriptor_sets.size()), descriptor_sets.data(), 0, nullptr);
	}
}

template void routine_impl::_update_descriptor_sets(const gpu::dispatch_info &info, const vk12::pipeline &pipeline);
template void routine_impl::_update_descriptor_sets(const gpu::draw_info &info, const vk12::pipeline &pipeline);

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

template<typename RoutineContext>
routine<RoutineContext>::routine(std::shared_ptr<gpu::device> &&device)
		: gpu::_internal::routine<RoutineContext>(std::move(device)),
		impl(*static_cast<vk12::device *>(this->_device.get())) {
	vk12::adapter &adapter = *static_cast<vk12::adapter *>(impl.vk12_device.adapter().get());

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

	if constexpr (std::is_same_v<RoutineContext, gpu::universal_routine_context>) {
		impl.queue = impl.vk12_device.universal_queue;
		pool_info.queueFamilyIndex = adapter.universal_family;
	} else if constexpr (std::is_same_v<RoutineContext, gpu::compute_routine_context>) {
		impl.queue = impl.vk12_device.compute_queue;
		pool_info.queueFamilyIndex = adapter.compute_family;
	} else {
		impl.queue = impl.vk12_device.copy_queue;
		pool_info.queueFamilyIndex = adapter.copy_family;
	}
	
	VOLT_VK12_CHECK(vkCreateCommandPool(impl.vk12_device.vk_device, &pool_info, nullptr, &impl.command_pool),
			"Failed to create command pool.")

	VkCommandBufferAllocateInfo command_buffer_info{};
	command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_info.commandPool = impl.command_pool;
	command_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buffer_info.commandBufferCount = 1;

	VOLT_VK12_CHECK(vkAllocateCommandBuffers(impl.vk12_device.vk_device, &command_buffer_info, &impl.command_buffer),
			"Failed to allocate command buffers.")

	VkSemaphoreTypeCreateInfo timeline_info{};
	timeline_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	timeline_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	timeline_info.initialValue = 0;

	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = &timeline_info;

	VOLT_VK12_CHECK(vkCreateSemaphore(impl.vk12_device.vk_device, &semaphore_info, nullptr,
			&impl.finish_semaphore), "Failed to create semaphore.")

	impl._add_descriptor_pool();
}

template<typename RoutineContext>
routine<RoutineContext>::~routine() {
	wait();

	for (VkDescriptorPool pool : impl.descriptor_pools) {
		vkDestroyDescriptorPool(impl.vk12_device.vk_device, pool, nullptr);
	}

	vkDestroySemaphore(impl.vk12_device.vk_device, impl.finish_semaphore, nullptr);
	vkFreeCommandBuffers(impl.vk12_device.vk_device, impl.command_pool, 1, &impl.command_buffer);
	vkDestroyCommandPool(impl.vk12_device.vk_device, impl.command_pool, nullptr);
}

template<typename RoutineContext>
void routine<RoutineContext>::execute(const std::function<void(RoutineContext &)> &callback) {
	VOLT_VK12_DEBUG_CHECK(vkResetCommandPool(impl.vk12_device.vk_device, impl.command_pool, 0),
			"Failed to reset command pool.")

	// Reset descriptor pools
	for (VkDescriptorPool pool : impl.descriptor_pools)
		vkResetDescriptorPool(impl.vk12_device.vk_device, pool, 0);
	impl.current_descriptor_pool = 0;

	// Begin command buffer
	VkCommandBufferBeginInfo begin_info{};
	begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	VOLT_VK12_DEBUG_CHECK(vkBeginCommandBuffer(impl.command_buffer, &begin_info),
			"Failed to begin command buffer.")
	
	// Execute user callback
	RoutineContext routine_context = RoutineContext::_new(impl);
	callback(routine_context);

	// End command buffer
	VOLT_VK12_DEBUG_CHECK(vkEndCommandBuffer(impl.command_buffer),
			"Failed to end command buffer.")

	std::vector<VkSemaphore> wait_semaphores;
	std::vector<VkPipelineStageFlags> wait_stages;
	std::vector<uint64_t> wait_values;

	wait_semaphores.reserve(impl.wait_semaphores.size());
	wait_stages.reserve(impl.wait_semaphores.size());
	wait_values.reserve(impl.wait_semaphores.size());

	for (auto &semaphore : impl.wait_semaphores) {
		wait_semaphores.push_back(semaphore.semaphore);
		wait_stages.push_back(semaphore.stage_mask);
		wait_values.push_back(semaphore.value);
	}

	// Submit
	impl.finish_value++;

	VkTimelineSemaphoreSubmitInfo timeline_info{};
	timeline_info.sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO;
	timeline_info.waitSemaphoreValueCount = static_cast<uint32_t>(wait_values.size());
	timeline_info.pWaitSemaphoreValues = wait_values.data();
	timeline_info.signalSemaphoreValueCount = 1;
	timeline_info.pSignalSemaphoreValues = &impl.finish_value;

	VkSubmitInfo submit_info = {};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.pNext = &timeline_info;
	submit_info.waitSemaphoreCount = static_cast<uint32_t>(impl.wait_semaphores.size());
	submit_info.pWaitSemaphores = wait_semaphores.data();
	submit_info.pWaitDstStageMask = wait_stages.data();
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &impl.command_buffer;
	submit_info.signalSemaphoreCount = 1;
	submit_info.pSignalSemaphores = &impl.finish_semaphore;

	VOLT_VK12_DEBUG_CHECK(vkQueueSubmit(impl.queue, 1, &submit_info, VK_NULL_HANDLE),
			"Failed to submit to queue.")
}

template<typename RoutineContext>
bool routine<RoutineContext>::finished() {
	// vkGetSemaphoreCounterValue does not provide access synchronization
	VkSemaphoreWaitInfo wait_info{};
	wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	wait_info.semaphoreCount = 1;
	wait_info.pSemaphores = &impl.finish_semaphore;
	wait_info.pValues = &impl.finish_value;

	VkResult result = vkWaitSemaphores(impl.vk12_device.vk_device, &wait_info, 0);
	if (result == VK_TIMEOUT)
		return false;

	VOLT_VK12_DEBUG_CHECK(result, "Failed to wait for semaphore.")
	return true;
}

template<typename RoutineContext>
void routine<RoutineContext>::wait() {
	VkSemaphoreWaitInfo wait_info{};
	wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	wait_info.semaphoreCount = 1;
	wait_info.pSemaphores = &impl.finish_semaphore;
	wait_info.pValues = &impl.finish_value;

	VOLT_VK12_DEBUG_CHECK(vkWaitSemaphores(impl.vk12_device.vk_device, &wait_info,
			std::numeric_limits<uint64_t>::max()), "Failed to wait for semaphore.")
}

template class vk12::routine<universal_routine_context>;
template class vk12::routine<compute_routine_context>;
template class vk12::routine<copy_routine_context>;

}
