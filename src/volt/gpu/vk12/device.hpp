#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/device.hpp>
#include <volt/gpu/pass.hpp>
#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/pass.hpp>

namespace volt::gpu::vk12 {

class device : public gpu::device {
public:
	VkDevice vk_device;
	VmaAllocator allocator;
	VkQueue universal_queue, compute_queue, copy_queue;
	VkPipelineCache pipeline_cache;

	device(std::shared_ptr<gpu::adapter> &&adapter);
	
	~device();

	void wait() override;

	std::shared_ptr<gpu::swapchain> create_swapchain(std::shared_ptr<os::window> window) override;

	std::shared_ptr<gpu::universal_routine> create_universal_routine() override;

	std::shared_ptr<gpu::compute_routine> create_compute_routine() override;

	std::shared_ptr<gpu::copy_routine> create_copy_routine() override;

	std::shared_ptr<gpu::buffer> create_buffer(
			gpu::memory_type memory_type,
			gpu::buffer_features features,
			size_t size) override;

	std::shared_ptr<gpu::texture> create_1d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, uint32_t size, uint32_t layers) override;

	std::shared_ptr<gpu::texture> create_2d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec2 size, uint32_t layers) override;

	std::shared_ptr<gpu::texture> create_3d_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec3 size) override;

	std::shared_ptr<gpu::texture> create_cube_texture(
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec2 size, uint32_t layers) override;

	std::shared_ptr<gpu::sampler> create_sampler(
			gpu::texture_filter filter, bool blur, float anisotropy) override;
};

}
