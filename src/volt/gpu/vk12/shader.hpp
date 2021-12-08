#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/shader.hpp>

namespace volt::gpu::vk12 {

class shader : public gpu::shader {
public:
	struct binding_point {
		uint32_t set, binding;
	};

	VkShaderModule shader_module;
	std::map<std::string, uint32_t> input_locations;
	std::map<std::string, binding_point> binding_points;

	shader(std::shared_ptr<gpu::device> &&device, const std::vector<uint8_t> &bytecode);

	~shader();
};

}
