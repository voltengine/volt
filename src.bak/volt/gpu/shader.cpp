#include <volt/pch.hpp>
#include <volt/gpu/shader.hpp>

namespace volt::gpu {

const std::shared_ptr<gpu::device> &shader::device() const {
	return _device;
}

shader::shader(std::shared_ptr<gpu::device> &&device)
		: _device(std::move(device)) {}

}
