#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/shader.hpp>

namespace volt::gpu::vk12 {

class shader : public gpu::shader {
public:
	VkShaderModule shader_module;

	shader(std::shared_ptr<gpu::device> &&device, const std::vector<uint8_t> &bytecode);

	~shader();
};

}
