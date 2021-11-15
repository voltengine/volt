#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/queue.hpp>

namespace volt::video::vk12 {

class queue : public video::queue {
public:
	VkQueue vk_queue;

	VOLT_API queue() = default;

	VOLT_API queue(VkQueue vk_queue);

	VOLT_API void wait() override;
};

}