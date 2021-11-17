#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/adapter.hpp>

namespace volt::gpu::d3d12 {

class adapter : public gpu::adapter {
public:
	IDXGIAdapter1 *d3d_adapter;
	DXGI_ADAPTER_DESC1 adapter_desc;

	adapter(std::shared_ptr<gpu::instance> &&instance, IDXGIAdapter1 *d3d_adapter);

	uint32_t vendor_id() override;

	uint32_t device_id() override;

	std::string name() override;

	uint64_t dedicated_video_memory() override;

	std::shared_ptr<gpu::device> create_device() override;
};

}
