#pragma once

#include <volt/pch.hpp>

#include <volt/math/math.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/enums.hpp>
#include <volt/gpu/texture.hpp>

namespace volt::gpu::vk12 {

class texture : public gpu::texture {
public:
	enum class type {
		tex1d, tex2d, tex3d, cube,
		tex1d_array, tex2d_array, cube_array
	};

	enum class state {
		undefined,
		copy_src,
		copy_dst,
		present,
		color_attachment,
		depth_stencil_attachment,
		rasterization_shared,
		rasterization_unique,
		compute_shared,
		compute_unique
	};

	struct access_pattern {
		VkPipelineStageFlags stage_mask;
		VkAccessFlags access_mask;
		VkImageLayout layout;
	};

	VkImage image;
	VkFormat vk_format;
	VmaAllocation allocation = VK_NULL_HANDLE;
	VkImageAspectFlags aspect_mask;
	VkImageView image_view;
	state current_state = state::undefined;
	access_pattern current_access_pattern = access_patterns[state::undefined];
	vk12::routine_impl *current_routine_impl = nullptr;
	VkSemaphore swapchain_acquire_semaphore = VK_NULL_HANDLE;

	texture(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::texture_features features,
			gpu::texture_format format,
			uint32_t levels, math::uvec3 size, type type);

	// Just for swapchain images
	texture(std::shared_ptr<gpu::device> &&device, gpu::texture_format format, math::uvec2 size, VkImage image);

	~texture();

	void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) override;

	void unmap(size_t write_offset = 0, size_t write_size = 0) override;

	void barrier(vk12::routine_impl &impl, state state);

private:
	static std::unordered_map<vk12::texture::state, access_pattern> access_patterns;

	vk12::device &vk12_device;
};

}