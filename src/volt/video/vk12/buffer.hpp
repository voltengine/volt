#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/vk12/device.hpp>
#include <volt/video/buffer.hpp>
#include <volt/video/queue.hpp>

namespace volt::video::vk12 {

class buffer : public video::buffer {
public:
	VkBuffer vk_buffer;
	VmaAllocation allocation;

	VOLT_API buffer(std::shared_ptr<video::device> &&device,
			video::resource::type resource_type,
			video::queue::types sync_queues,
			video::buffer::features features,
			size_t size);

	VOLT_API ~buffer();

	VOLT_API void map(void **ptr) override;

	VOLT_API void unmap() override;

	VOLT_API void read(size_t offset, size_t size) override;

	VOLT_API void write(size_t offset, size_t size) override;

private:
	vk12::device &_device;
};

}