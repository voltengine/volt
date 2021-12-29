#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>

namespace volt::gpu {

class device;

enum class sampler_filter {
	nearest, linear, trilinear
};

// Sampler cannot be accessed when it's modified
// Use sampler->device()->wait() to stall until the sampler is available
class sampler {
public:
	VOLT_API virtual ~sampler();

	VOLT_API const std::shared_ptr<gpu::device> &device() const;

	VOLT_API sampler_filter filter() const;

	VOLT_API void filter(sampler_filter filter);

	VOLT_API bool blur() const;

	VOLT_API void blur(bool blur);

	VOLT_API float anisotropy() const;

	VOLT_API void anisotropy(float anisotropy);

protected:
	std::shared_ptr<gpu::device> _device;
	sampler_filter _filter;
	bool _blur;
	float _anisotropy;

	VOLT_API sampler(std::shared_ptr<gpu::device> &&device, sampler_filter filter, bool blur, float anisotropy);

	virtual void create() {}

	virtual void destroy() {}
};

}
