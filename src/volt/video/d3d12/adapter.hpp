#pragma once

#include <volt/pch.hpp>

#include <volt/video/adapter.hpp>

namespace volt::video::d3d12 {

class adapter : public video::adapter {
public:
	IDXGIAdapter1 *d3d_adapter;
	DXGI_ADAPTER_DESC1 adapter_desc;

	adapter(std::shared_ptr<video::instance> &&instance, IDXGIAdapter1 *d3d_adapter);

	uint32_t vendor_id() override;

	uint32_t device_id() override;

	std::string name() override;

	uint64_t dedicated_video_memory() override;

	std::shared_ptr<video::device> create_device() override;
};

}
