#include <volt/pch.hpp>
#include <volt/gpu/resource.hpp>

namespace volt::gpu {

const std::shared_ptr<gpu::device> &resource::device() const {
	return _device;
}

resource::resource(std::shared_ptr<gpu::device> &&device)
		: _device(std::move(device)) {}

}
