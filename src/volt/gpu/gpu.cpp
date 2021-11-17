#include <volt/pch.hpp>
#include <volt/gpu/gpu.hpp>

#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu {

std::shared_ptr<gpu::instance> create_instance(gpu::api api) {
	switch (api) {
		default:
		case gpu::api::vk12:
			return std::make_shared<vk12::instance>();
	}
}

}
