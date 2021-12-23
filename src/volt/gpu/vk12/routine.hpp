#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/routine.hpp>

namespace volt::gpu::vk12 {

class device;

class routine_impl : public gpu::_internal::routine_impl {
public:
	vk12::device &vk12_device;
	VkQueue queue;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	// VkSemaphore finish_semaphore;
	VkFence finish_fence;
	// ...
	std::vector<VkDescriptorPool> descriptor_pools;
	size_t current_pool = 0;

	// Statistics for optimizing descriptor pool sizes
	uint32_t sets_allocated_from_current_pool             = 1;
	uint32_t constant_buffers_allocated_from_current_pool = 1;
	uint32_t sampled_textures_allocated_from_current_pool = 1;
	uint32_t storage_buffers_allocated_from_current_pool  = 1;
	uint32_t storage_textures_allocated_from_current_pool = 1;

	~routine_impl();

	template<typename Executor>
	static routine_impl _new(vk12::device &vk12_device);

	VOLT_API void copy_buffer(
			const std::shared_ptr<gpu::buffer> &src,
			const std::shared_ptr<gpu::buffer> &dst,
			size_t src_offset,
			size_t dst_offset,
			size_t size) override;

	VOLT_API void copy_texture(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst) override;

	VOLT_API void copy_texture_level(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst,
			uint32_t src_level,
			uint32_t dst_level,
			math::uvec3 src_offset,
			math::uvec3 dst_offset,
			math::uvec3 size) override;

	VOLT_API void compute_pass(const compute_pass_info &info,
			const std::function<void(gpu::compute_pass &)> &callback) override;

	VOLT_API void rasterization_pass(const rasterization_pass_info &info,
			const std::function<void(gpu::rasterization_pass &)> &callback) override;
	
	VOLT_API std::vector<VkDescriptorSet> _allocate_descriptor_sets(const std::vector<VkDescriptorSetLayout> &layouts,
			uint32_t constant_buffer_count, uint32_t sampled_texture_count, uint32_t storage_buffer_count, uint32_t storage_texture_count);

private:
	VOLT_API routine_impl(vk12::device &vk12_device);

	void _add_descriptor_pool();
};

template<typename Executor>
class routine : public gpu::_internal::routine<Executor> {
public:
	routine_impl impl;

	routine(std::shared_ptr<gpu::device> &&device);

	~routine();

	void execute(const std::function<void(Executor &)> &callback) override;

	bool finished() override;

	void wait() override;
};

using universal_routine = routine<gpu::universal_executor>;
using compute_routine = routine<gpu::compute_executor>;
using copy_routine = routine<gpu::copy_executor>;

}
