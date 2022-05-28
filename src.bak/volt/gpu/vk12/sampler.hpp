#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/sampler.hpp>

namespace volt::gpu::vk12 {

class sampler : public gpu::sampler {
public:
	VkSampler vk_sampler;

	sampler(std::shared_ptr<gpu::device> &&device, sampler_filter filter, bool blur, float anisotropy);

protected:
	void create() override;

	void destroy() override;
};

}
