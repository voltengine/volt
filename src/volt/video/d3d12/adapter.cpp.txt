#include <volt/pch.hpp>
#include <volt/video/d3d12/adapter.hpp>

#include <volt/video/d3d12/device.hpp>

namespace volt::video::d3d12::_internal {

adapter::adapter(IDXGIAdapter1 *d3d_adapter) : d3d_adapter(d3d_adapter) {
	d3d_adapter->GetDesc1(&d3d_adapter_desc);
}

adapter::~adapter() {
	d3d_adapter->Release();
}

std::shared_ptr<video::device> adapter::create_device() {
	return std::make_shared<_internal::device>(d3d_adapter);
}

uint32_t adapter::vendor_id() {
	return d3d_adapter_desc.VendorId;
}

uint32_t adapter::device_id() {
	return d3d_adapter_desc.DeviceId;
}

std::string adapter::name() {
	std::wstring name(d3d_adapter_desc.Description);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(name);
}

uint64_t adapter::dedicated_video_memory() {
	return d3d_adapter_desc.DedicatedVideoMemory;
}

}
