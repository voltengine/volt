#pragma once

#include "../macros.hpp"

#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device;
class swapchain;

class swapchain_frame {
public:
	~swapchain_frame();

	bool acquired() const;

	uint32_t index() const;

	const std::shared_ptr<gpu::texture> &texture() const;

private:
	friend gpu::swapchain;

	gpu::swapchain *swapchain;
	uint32_t index;

	swapchain_frame(gpu::swapchain *swapchain, uint32_t index
			= std::numeric_limits<uint32_t>::max());
};

class swapchain : public std::enable_shared_from_this<swapchain> {
public:
	virtual ~swapchain() = default;

	virtual swapchain_frame acquire() = 0;

	const std::shared_ptr<gpu::device> &get_device() {
		return device;
	}

	const std::shared_ptr<os::window> &get_window() {
		return window;
	}

protected:
	std::shared_ptr<gpu::device> device;
	std::shared_ptr<os::window> window;

	swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
			: device(std::move(device)), window(std::move(window)) {}
};

}
