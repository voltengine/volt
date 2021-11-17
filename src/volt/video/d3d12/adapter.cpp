#include <volt/video/d3d12/adapter.hpp>

#include <volt/video/d3d12/device.hpp>

namespace volt::video::d3d12 {

adapter::adapter(std::shared_ptr<video::instance> &&instance, IDXGIAdapter1 *d3d_adapter)
		: video::adapter(std::move(instance)), d3d_adapter(d3d_adapter) {
	d3d_adapter->GetDesc1(&adapter_desc);
}

uint32_t adapter::vendor_id() {
	return adapter_desc.VendorId;
}

uint32_t adapter::device_id() {
	return adapter_desc.DeviceId;
}

std::string adapter::name() {
	std::wstring name(adapter_desc.Description);
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.to_bytes(name);
}

uint64_t adapter::dedicated_video_memory() {
	return adapter_desc.DedicatedVideoMemory;
}

std::shared_ptr<video::device> adapter::create_device() {
	return std::shared_ptr<video::device>(
			new d3d12::device(shared_from_this()));
}

}
