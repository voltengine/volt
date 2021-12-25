#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/instance.hpp>

namespace volt::gpu::gl46 {

class instance : public gpu::instance {
public:
	std::vector<std::shared_ptr<gpu::adapter>> enumerate_adapters() override;

	uint32_t concurrent_frames() override;
};

}
