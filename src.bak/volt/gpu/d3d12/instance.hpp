#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/instance.hpp>

namespace volt::gpu::d3d12 {

class instance : public gpu::instance {
public:
	IDXGIFactory4 *factory;
	
#ifdef VOLT_GPU_DEBUG
	ID3D12Debug1 *debug;
#endif

	instance();

	~instance();

	std::vector<std::shared_ptr<gpu::adapter>> enumerate_adapters() override;
};

}
