#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/buffer.hpp>

namespace volt::gpu::vk12 {

class buffer : public gpu::buffer {
public:
	VkBuffer vk_buffer;
	VmaAllocation allocation;

	buffer(std::shared_ptr<gpu::device> &&device,
			gpu::memory_type memory_type,
			gpu::command_types sync_queues,
			gpu::buffer_features features,
			size_t size);

	~buffer();

	void map(void **ptr, size_t read_offset = 0, size_t read_size = 0) override;

	void unmap(size_t write_offset = 0, size_t write_size = 0) override;

private:
	vk12::device &_device;
};

}