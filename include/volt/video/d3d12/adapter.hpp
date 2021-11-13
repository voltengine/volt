#pragma once

#include "../../macros.hpp"

#include <dxgi.h>

#include "../adapter.hpp"

namespace volt::video::d3d12::_internal {

class adapter : public video::adapter {
public:
	VOLT_API adapter(IDXGIAdapter1 *d3d_adapter);

	VOLT_API ~adapter();

	VOLT_API std::shared_ptr<video::device> create_device() override;

	VOLT_API uint32_t vendor_id() override;

	VOLT_API uint32_t device_id() override;

	VOLT_API std::string name() override;

	VOLT_API uint64_t dedicated_video_memory() override;

private:
	IDXGIAdapter1 *d3d_adapter;
	DXGI_ADAPTER_DESC1 d3d_adapter_desc;
};

}
