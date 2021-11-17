#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/buffer.hpp>
#include <volt/gpu/queue.hpp>

namespace volt::gpu::vk12 {

class buffer : public gpu::buffer {
public:
	VkBuffer vk_buffer;
	VmaAllocation allocation;

	buffer(std::shared_ptr<gpu::device> &&device,
			gpu::resource_type resource_type,
			gpu::sync_queues sync_queues,
			gpu::buffer_features features,
			size_t size);

	~buffer();

	void map(void **ptr) override;

	void unmap() override;

	void read(size_t offset, size_t size) override;

	void write(size_t offset, size_t size) override;

private:
	vk12::device &_device;
};

}