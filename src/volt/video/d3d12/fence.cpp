#include <volt/video/d3d12/fence.hpp>

#include <volt/video/d3d12/d3d12.hpp>

namespace volt::video::d3d12 {

fence::fence(std::shared_ptr<video::device> &&device, uint64_t initial_value)
		: video::fence(std::move(device)) {
	ID3D12Device *d3d_device = static_cast<d3d12::device *>(this->device.get())->d3d_device;

	VOLT_D3D12_CHECK(d3d_device->CreateFence(initial_value, D3D12_FENCE_FLAG_NONE,
			IID_PPV_ARGS(&d3d_fence)), "Failed to create fence.")

	wait_event = CreateEventEx(0, 0, 0, EVENT_ALL_ACCESS);
}

fence::~fence() {
	d3d_fence->Release();
}

uint64_t fence::value() {
	return d3d_fence->GetCompletedValue();
}

void fence::signal(uint64_t value) {
	VOLT_D3D12_CHECK(d3d_fence->Signal(value), "Failed to signal fence.")
}

void fence::wait(uint64_t value) {
	d3d_fence->SetEventOnCompletion(value, wait_event);
	WaitForSingleObject(wait_event, INFINITE);
}

}
