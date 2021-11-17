#pragma once

#include <volt/pch.hpp>

#include <volt/video/fence.hpp>

namespace volt::video::vk12 {

class fence : public video::fence {
public:
	VkSemaphore semaphore;
	VkDevice vk_device;

	fence(std::shared_ptr<video::device> &&device, uint64_t initial_value = 0);

	~fence();

	uint64_t value() override;

	void signal(uint64_t value) override;

	void wait(uint64_t value) override;
};

}
