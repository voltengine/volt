#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pipeline.hpp>

namespace volt::gpu::vk12 {

class pipeline : public gpu::pipeline {
public:
	VKPipeline vk_pipeline;

	pipeline(std::shared_ptr<gpu::device> &&device,
			const std::shared_ptr<gpu::shader> &shader);

	~pipeline();
};

}