#include <volt/pch.hpp>
#include <volt/gpu/adapter.hpp>

namespace volt::gpu {

const std::shared_ptr<gpu::instance> &adapter::instance() const {
	return _instance;
}

adapter::adapter(std::shared_ptr<gpu::instance> &&instance)
		: _instance(std::move(instance)) {}

}
