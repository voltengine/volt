#include <volt/pch.hpp>
#include <volt/gpu/gpu.hpp>

#include <volt/gpu/d3d12/instance.hpp>
#include <volt/gpu/vk12/instance.hpp>

namespace volt::gpu {

std::shared_ptr<gpu::instance> create_instance(gpu::api api) {
	switch (api) {
		default:
		case gpu::api::d3d12:
			return std::make_shared<d3d12::instance>();
		case gpu::api::vk12:
			return std::make_shared<vk12::instance>();
	}
}

}
