#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/buffer.hpp>

namespace volt::gpu::vk12 {

class buffer : public gpu::buffer {
public:
	enum class state {
		undefined,
		copy_src,
		copy_dst,
		index_input,
		vertex_input,
		rasterization_shared,
		rasterization_unique,
		compute_shared,
		compute_unique
	};
	
	struct access_pattern {
		VkPipelineStageFlags stage_mask;
		VkAccessFlags access_mask;
	};

	VkBuffer vk_buffer;
	VmaAllocation allocation;
	state current_state = state::undefined;
	access_pattern current_access_pattern = access_patterns[state::undefined];

	buffer(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::buffer_features features,
			size_t size);

	~buffer();

	void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) override;

	void unmap(size_t write_offset = 0, size_t write_size = 0) override;

	void barrier(VkCommandBuffer command_buffer, state state);

private:
	static std::unordered_map<vk12::buffer::state, access_pattern> access_patterns;

	vk12::device &vk12_device;
};

}