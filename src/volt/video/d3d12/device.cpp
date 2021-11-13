#include <volt/pch.hpp>
#include <volt/video/d3d12/device.hpp>

#include <volt/video/d3d12/pipeline.hpp>
#include <volt/video/d3d12/shader.hpp>
#include <volt/error.hpp>

namespace volt::video::d3d12::_internal {

device::device(IDXGIAdapter1 *adapter) {
	VOLT_ASSERT(D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0,
			IID_PPV_ARGS(&d3d_device)) == S_OK, "Failed to create device.")
}

device::~device() {
	d3d_device->Release();
}

std::shared_ptr<video::swapchain> device::create_swapchain(const std::shared_ptr<video::window> &window) {
	// return std::make_shared<_internal::swapchain>();
	return nullptr;
}

std::shared_ptr<video::shader> device::create_shader(const std::vector<uint8_t> &bytecode) {
	return std::make_shared<_internal::shader>(d3d_device, bytecode);
}

std::shared_ptr<video::pipeline> device::create_pipeline() {
	return std::make_shared<_internal::pipeline>(d3d_device);
}

}
