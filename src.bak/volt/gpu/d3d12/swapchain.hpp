#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/d3d12/device.hpp>
#include <volt/gpu/queue.hpp>
#include <volt/gpu/swapchain.hpp>

namespace volt::gpu::d3d12 {

class swapchain : public gpu::swapchain {
public:
	IDXGISwapChain1 *d3d_swapchain;
	std::shared_ptr<gpu::rasterization_queue> queue;

	swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window);

	~swapchain();

	// virtual std::vector<const std::shared_ptr<gpu::texture>> &get_frames() = 0;
};

}
