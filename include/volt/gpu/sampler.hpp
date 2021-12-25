#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>

#include "enums.hpp"

namespace volt::gpu {

class device;

// Sampler cannot be accessed when it's modified
// Use sampler->device()->wait() to stall until the sampler is available
class sampler {
public:
	VOLT_API virtual ~sampler();

	VOLT_API const std::shared_ptr<gpu::device> &device();

	VOLT_API texture_filter filter();

	VOLT_API void filter(texture_filter filter);

	VOLT_API bool blur();

	VOLT_API void blur(bool blur);

	VOLT_API float anisotropy();

	VOLT_API void anisotropy(float anisotropy);

protected:
	std::shared_ptr<gpu::device> _device;
	texture_filter _filter;
	bool _blur;
	float _anisotropy;

	VOLT_API sampler(std::shared_ptr<gpu::device> &&device, texture_filter filter, bool blur, float anisotropy);

	virtual void create() {}

	virtual void destroy() {}
};

}
