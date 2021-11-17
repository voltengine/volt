#pragma once

#include <volt/pch.hpp>

#include <volt/video/vk12/device.hpp>
#include <volt/video/buffer.hpp>
#include <volt/video/queue.hpp>

namespace volt::video::vk12 {

class buffer : public video::buffer {
public:
	VkBuffer vk_buffer;
	VmaAllocation allocation;

	buffer(std::shared_ptr<video::device> &&device,
			video::resource_type resource_type,
			video::sync_queues sync_queues,
			video::buffer_features features,
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