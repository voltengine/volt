#pragma once

#include "../../macros.hpp"

#include <d3d12.h>
#include <dxgi.h>

#include "../device.hpp"

namespace volt::video::d3d12::_internal {

class device : public video::device {
public:
	VOLT_API device(IDXGIAdapter1 *d3d_adapter);

	VOLT_API ~device();

	VOLT_API std::shared_ptr<video::swapchain> create_swapchain(const std::shared_ptr<video::window> &window) override;

	VOLT_API std::shared_ptr<video::shader> create_shader(const std::vector<uint8_t> &bytecode) override;

	VOLT_API std::shared_ptr<video::pipeline> create_pipeline() override;

private:
	ID3D12Device *d3d_device;
};


}
