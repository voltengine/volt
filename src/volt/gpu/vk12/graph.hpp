#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/graph.hpp>

namespace volt::gpu::vk12 {

class graph : public gpu::graph {
public:
	struct thread_data {
		VkCommandPool command_pool;
		std::vector<VkCommandBuffer> primary_command_buffers;
		std::vector<VkCommandBuffer> secondary_command_buffers;
		std::vector<VkDescriptorPool> descriptor_pools;

		size_t current_primary_command_buffer = 0;
		size_t current_secondary_command_buffer = 0;
		size_t current_descriptor_pool = 0;

		// Descriptor pool allocation heuristics
		uint32_t allocated_sets             = 1;
		uint32_t allocated_constant_buffers = 1;
		uint32_t allocated_sampled_textures = 1;
		uint32_t allocated_storage_buffers  = 1;
		uint32_t allocated_storage_textures = 1;
	};

	vk12::device &vk12_device;

	VkSemaphore finish_semaphore;
	uint64_t finish_value = 0;

	VkRenderPass current_render_pass = VK_NULL_HANDLE;

	// Thread zero shares data with the main
	// thread which always waits for thread pool
	std::vector<thread_data> threads;

	void copy(copy_info &copy_info) override;

	void dispatch(const dispatch_info &info) override;

	void pass(const pass_info &info,
			const std::function<void(gpu::pass &)> &callback) override;

	void async_pass(const pass_info &info,
			const std::function<void(gpu::async_pass &)> &callback) override;
};

}
