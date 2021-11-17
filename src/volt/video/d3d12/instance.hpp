#pragma once

#include <volt/pch.hpp>

#include <volt/video/instance.hpp>

namespace volt::video::d3d12 {

class instance : public video::instance {
public:
	IDXGIFactory4 *factory;
	
#ifdef VOLT_VIDEO_DEBUG
	ID3D12Debug1 *debug;
#endif

	instance();

	~instance();

	std::vector<std::shared_ptr<video::adapter>> enumerate_adapters() override;
};

}
