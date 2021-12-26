#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/routine.hpp>
#include <volt/gpu/vk12/jit.hpp>

namespace volt::gpu::vk12 {

class device;

class routine_impl : public gpu::_internal::routine_impl {
public:
	struct wait_semaphore {
		VkSemaphore semaphore;
		VkPipelineStageFlags stage_mask;
		uint64_t value = 0; // 0 means binary semaphore

		auto operator<=>(const wait_semaphore &other) const = default;
	};

	vk12::device &vk12_device;
	VkQueue queue;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	VkSemaphore finish_semaphore;
	uint64_t finish_value = 0;
	std::set<wait_semaphore> wait_semaphores;
	std::vector<VkDescriptorPool> descriptor_pools;
	size_t current_pool = 0;
	VkRenderPass current_render_pass = VK_NULL_HANDLE;

	// Statistics for optimizing descriptor pool sizes
	uint32_t sets_allocated_from_current_pool             = 1;
	uint32_t constant_buffers_allocated_from_current_pool = 1;
	uint32_t sampled_textures_allocated_from_current_pool = 1;
	uint32_t storage_buffers_allocated_from_current_pool  = 1;
	uint32_t storage_textures_allocated_from_current_pool = 1;

	routine_impl(vk12::device &vk12_device);

	void copy_buffer(
			const std::shared_ptr<gpu::buffer> &src,
			const std::shared_ptr<gpu::buffer> &dst,
			size_t src_offset,
			size_t dst_offset,
			size_t size) override;

	void copy_texture(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst) override;

	void copy_texture_level(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst,
			uint32_t src_level,
			uint32_t dst_level,
			math::uvec3 src_offset,
			math::uvec3 dst_offset,
			math::uvec3 size) override;

	void dispatch(const dispatch_info &info) override;

	void pass(const pass_info &info,
			const std::function<void(gpu::pass_context &)> &callback) override;

	void async_pass(const pass_info &info,
			const std::function<void(gpu::async_pass_context &)> &callback) override;
	
	template<typename DescriptorInfo> // dispatch_info / draw_info
	void _update_descriptor_sets(const DescriptorInfo &info, const pipeline &pipeline);

	void _add_descriptor_pool();
};

template<typename RoutineContext>
class routine : public gpu::_internal::routine<RoutineContext> {
public:
	routine_impl impl;

	routine(std::shared_ptr<gpu::device> &&device);

	~routine();

	void execute(const std::function<void(RoutineContext &)> &callback) override;

	bool finished() override;

	void wait() override;
};

using universal_routine = routine<gpu::universal_routine_context>;
using compute_routine = routine<gpu::compute_routine_context>;
using copy_routine = routine<gpu::copy_routine_context>;

}
