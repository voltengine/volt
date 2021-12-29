#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>
#include <vector>

#include "../os/os.hpp"
#include "texture.hpp"

namespace volt::gpu {

class device;
class swapchain;

enum class present_mode {
	tear,         // Results in tearing; Has no input lag
	vsync,        // Big input lag + clamps to 30 fps for any framerate below 60
	triple_buffer // Small input lag + Higher energy consumption (unavailable with OpenGL)
};

struct frame {
	const uint32_t index; // The index of the frame in flight
	std::shared_ptr<gpu::texture> &texture; // Output texture
};

class swapchain : public std::enable_shared_from_this<swapchain> {
public:
	virtual ~swapchain() = default;

	virtual void frame(std::function<void(frame)> &&callback) = 0;

	VOLT_API float framerate_limit() const;

	VOLT_API void framerate_limit(float fps);

	VOLT_API gpu::present_mode present_mode() const;

	VOLT_API void present_mode(gpu::present_mode mode);

	VOLT_API const std::shared_ptr<gpu::device> &device() const;

	VOLT_API const std::shared_ptr<os::window> &window() const;

protected:
	std::shared_ptr<gpu::device> _device;
	std::shared_ptr<os::window> _window;

	float frame_time = 0;
	util::timer framerate_timer;
	uint32_t current_frame = 0;
	gpu::present_mode _present_mode = gpu::present_mode::triple_buffer; // Will be adjusted during construction

	std::vector<std::shared_ptr<gpu::texture>> textures;

	swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window);

	virtual void create() {};

	virtual void destroy() {};
};

}
