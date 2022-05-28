#pragma once

#include "../macros.hpp"

#include <functional>
#include <memory>

namespace volt::gpu {

class device;

using access_modes = uint8_t;
namespace access_mode {
	constexpr access_modes
			read  = 1 << 0,
			write = 1 << 1,
			read_write = read | write;
};

class resource {
public:
	virtual ~resource() = default;

	template<typename T>
	void map(const std::function<void(T &)> &callback, access_modes access, size_t offset = 0);

	virtual void map(const std::function<void(void *)> &callback, access_modes access, size_t size = 0, size_t offset = 0) = 0;

	VOLT_API const std::shared_ptr<gpu::device> &device() const;
	
protected:
	std::shared_ptr<gpu::device> _device;

	resource(std::shared_ptr<gpu::device> &&device);
};

}

#include "resource.inl"
