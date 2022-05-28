#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/instance.hpp>

namespace volt::gpu::vk12 {

class instance : public gpu::instance {
public:
	VkInstance vk_instance;
#ifdef VOLT_GPU_DEBUG
	VkDebugUtilsMessengerEXT vk_messenger;
#endif

	instance();

	~instance();

	std::vector<std::shared_ptr<gpu::adapter>> enumerate_adapters() override;

	uint32_t graph_count() const override;
};

}
