#include <volt/pch.hpp>
#include <volt/gpu/device.hpp>

namespace volt::gpu {

const std::shared_ptr<gpu::adapter> &device::adapter() {
	return _adapter;
}

device::device(std::shared_ptr<gpu::adapter> &&adapter)
			: _adapter(std::move(adapter)) {}

}
