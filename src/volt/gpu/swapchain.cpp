#include <volt/pch.hpp>
#include <volt/gpu/swapchain.hpp>

namespace volt::gpu {

swapchain_frame::~swapchain_frame() {
	if (!swapchain)
		return;
	
	// Transition texture to present
	// Signal fence
	// Flush
}

bool swapchain_frame::acquired() const {
	return swapchain != nullptr;
}

uint32_t swapchain_frame::index() const {
	return index;
}

const std::shared_ptr<gpu::texture> &swapchain_frame::texture() const {
	//return swapchain->textures[index];
	return nullptr; // TODO: Implement
}

swapchain_frame::swapchain_frame(gpu::swapchain *swapchain, uint32_t index)
		: swapchain(swapchain), index(index) {}

}
