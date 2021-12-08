#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/swapchain.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

class swapchain : public gpu::swapchain {
public:
	VkDevice vk_device;
	VkSurfaceKHR surface;
	VkSwapchainKHR vk_swapchain;
	math::uvec2 frame_size;
	std::vector<VkSemaphore> frame_begin_semaphores = std::vector<VkSemaphore>(vk12::concurrent_frames);
	std::vector<VkSemaphore> frame_end_semaphores = std::vector<VkSemaphore>(vk12::concurrent_frames);

	swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window);

	~swapchain();

	void next_frame(std::function<void(frame)> &&callback) override;

protected:
	void reconstruct() override;
};

}
