#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/routine.hpp>

namespace volt::gpu::vk12 {

template<typename Executor>
class routine_impl : public gpu::_internal::routine_impl {
public:
	vk12::device &vk12_device;
	VkQueue queue;
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
	// VkSemaphore finish_semaphore;
	VkFence finish_fence;
	// ...

	routine_impl(vk12::device &vk12_device);

	~routine_impl();

	void copy_buffer(
			const std::shared_ptr<gpu::buffer> &src,
			const std::shared_ptr<gpu::buffer> &dst,
			size_t src_offset,
			size_t dst_offset,
			size_t size) override;

	VOLT_API void copy_texture_level(
			const std::shared_ptr<gpu::texture> &src,
			const std::shared_ptr<gpu::texture> &dst,
			uint32_t src_level,
			uint32_t dst_level,
			math::uvec3 src_offset,
			math::uvec3 dst_offset,
			math::uvec3 size) override;

	void compute_pass(const compute_pass_info &info,
			const std::function<void(gpu::compute_pass &)> &callback) override;

	void rasterization_pass(const rasterization_pass_info &info,
			const std::function<void(gpu::rasterization_pass &)> &callback) override;
};

template<typename Executor>
class routine : public gpu::_internal::routine<Executor> {
public:
	routine_impl<Executor> impl;

	routine(std::shared_ptr<gpu::device> &&device);

	~routine();

	void execute(const std::function<void(Executor &)> &callback) override;

	bool finished() override;

	void wait() override;
};

using graphics_routine = routine<gpu::graphics_executor>;
using compute_routine = routine<gpu::compute_executor>;
using copy_routine = routine<gpu::copy_executor>;

}
