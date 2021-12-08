#include <volt/gpu/vk12/shader.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

shader::shader(std::shared_ptr<gpu::device> &&device, const std::vector<uint8_t> &bytecode)
		: gpu::shader(std::move(device)) {
	SpvReflectShaderModule reflection;
	VOLT_ASSERT(spvReflectCreateShaderModule(bytecode.size(), bytecode.data(), &reflection)
			== SPV_REFLECT_RESULT_SUCCESS, "Failed to reflect shader module.")

	for (uint32_t i = 0; i < reflection.input_variable_count; i++) {
		auto *var = reflection.input_variables[i];
		input_locations[var->name] = var->location;
	}

	for (uint32_t i = 0; i < reflection.descriptor_set_count; i++) {
		auto set = reflection.descriptor_sets[i];
		for (uint32_t j = 0; j < set.binding_count; j++) {
			auto *binding = set.bindings[j];
			binding_points[binding->name] = { set.set, binding->binding };
		}
	}
			
	VkDevice vk_device = static_cast<vk12::device *>(this->_device.get())->vk_device;

	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = bytecode.size();
	info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

	// vkCreateShaderModule makes its own copy of bytecode
	VOLT_VK12_CHECK(vkCreateShaderModule(vk_device, &info, nullptr, &shader_module),
			"Failed to create shader module.")
}

shader::~shader() {
	VkDevice vk_device = static_cast<vk12::device *>(_device.get())->vk_device;
	vkDestroyShaderModule(vk_device, shader_module, nullptr);
}

}
