#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/fence.hpp>

namespace volt::gpu::vk12 {

class fence : public gpu::fence {
public:
	VkSemaphore semaphore;
	VkDevice vk_device;

	fence(std::shared_ptr<gpu::device> &&device, uint64_t initial_value = 0);

	~fence();

	uint64_t value() override;

	void signal(uint64_t value) override;

	void wait(uint64_t value) override;
};

}
