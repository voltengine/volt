#include <volt/gpu/vk12/shader.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

shader::shader(std::shared_ptr<gpu::device> &&device, const std::vector<uint8_t> &bytecode)
		: gpu::shader(std::move(device)) {
	SpvReflectShaderModule refl_module;
	VOLT_ASSERT(spvReflectCreateShaderModule(bytecode.size(), bytecode.data(), &refl_module)
			== SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module.")
			
	VkDevice vk_device = static_cast<vk12::device *>(this->_device.get())->vk_device;

	// Shader module
	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = bytecode.size();
	info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

	// vkCreateShaderModule makes its own copy of bytecode
	VOLT_VK12_CHECK(vkCreateShaderModule(vk_device, &info, nullptr, &shader_module),
			"Failed to create shader module.")
	
	// Stage info dos not copy entry point name from reflection, so we must cache it
	stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stage_info.stage = static_cast<VkShaderStageFlagBits>(refl_module.shader_stage);
	stage_info.module = shader_module;
	stage_info.pName = (entry_point_name = refl_module.entry_point_name).c_str();

	// Descriptor set layout
	set_layout_infos.resize(refl_module.descriptor_set_count);

	for (uint32_t i = 0; i < refl_module.descriptor_set_count; i++) {
		SpvReflectDescriptorSet refl_set = refl_module.descriptor_sets[i];

		set_layout_infos[i].set = refl_set.set;
		std::vector<VkDescriptorSetLayoutBinding> &layout_bindings = set_layout_infos[i].bindings;
		layout_bindings.resize(refl_set.binding_count);

		for (uint32_t j = 0; j < refl_set.binding_count; j++) {
			SpvReflectDescriptorBinding &refl_binding = *refl_set.bindings[j];
			binding_points[refl_binding.name] = { refl_set.set, refl_binding.binding };

			VkDescriptorSetLayoutBinding &layout_binding = layout_bindings[j];
			layout_binding.binding = refl_binding.binding;
    		layout_binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);
    		layout_binding.descriptorCount = 1;
			for (uint32_t k = 0; k < refl_binding.array.dims_count; k++)
				layout_binding.descriptorCount *= refl_binding.array.dims[k];
    		layout_binding.stageFlags = static_cast<VkShaderStageFlagBits>(refl_module.shader_stage);
		}
	}

	if (refl_module.shader_stage != SpvReflectShaderStageFlagBits::SPV_REFLECT_SHADER_STAGE_VERTEX_BIT)
		return;

	// Only for vertex shaders
	for (uint32_t i = 0; i < refl_module.input_variable_count; i++) {
		SpvReflectInterfaceVariable &refl_var = *refl_module.input_variables[i];
		vertex_inputs[refl_var.name] = { refl_var.location, static_cast<VkFormat>(refl_var.format) };	
	}
}

shader::~shader() {
	VkDevice vk_device = static_cast<vk12::device *>(_device.get())->vk_device;
	vkDestroyShaderModule(vk_device, shader_module, nullptr);
}

}
