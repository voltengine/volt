#pragma once

#include <volt/pch.hpp>

#include <volt/video/pipeline.hpp>

namespace volt::video::vk12 {

class pipeline : public video::pipeline {
public:
	VKPipeline vk_pipeline;

	pipeline(std::shared_ptr<video::device> &&device,
			const std::shared_ptr<video::shader> &shader);

	~pipeline();
};

}