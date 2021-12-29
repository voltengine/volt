#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/swapchain.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

class swapchain : public gpu::swapchain {
public:
	vk12::device &vk12_device;
	VkSurfaceKHR surface;
	VkSwapchainKHR vk_swapchain;
	math::uvec2 frame_size;
	VkSemaphore acquire_semaphores[vk12::graph_count];

	swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window);

	~swapchain();

	void next_frame(std::function<void(frame)> &&callback) override;

protected:
	void create() override;

	void destroy() override;
};

}
