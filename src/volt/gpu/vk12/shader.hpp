#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/shader.hpp>

namespace volt::gpu::vk12 {

class shader : public gpu::shader {
public:
	// VkDescriptorSetLayout is created from multiple merged set_layout_infos
	struct set_layout_info {
		uint32_t set;
		std::vector<VkDescriptorSetLayoutBinding> bindings;
	};

	struct binding_point {
		uint32_t set, binding;
	};

// TODO: Support matrices, double precision vec3 and arrays using multipler locations per input
	struct vertex_input {
		uint32_t location;
		VkFormat format;
	};

	VkShaderModule shader_module;
	std::string entry_point_name;
	VkPipelineShaderStageCreateInfo stage_info;
	std::vector<set_layout_info> set_layout_infos;
	std::unordered_map<std::string, binding_point> binding_points;
	std::unordered_map<std::string, vertex_input> vertex_inputs;

	shader(std::shared_ptr<gpu::device> &&device, const std::vector<uint8_t> &bytecode);

	~shader();
};

}
