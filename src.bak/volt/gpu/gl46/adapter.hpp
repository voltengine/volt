#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/adapter.hpp>
#include <volt/gpu/instance.hpp>

namespace volt::gpu::gl46 {

class adapter : public gpu::adapter {
public:
	adapter(std::shared_ptr<gpu::instance> &&instance);

	std::string vendor() override;

	std::string name() override;

	uint64_t total_memory() override;

	uint64_t free_memory() override;

	std::shared_ptr<gpu::device> create_device() override;
};

}
