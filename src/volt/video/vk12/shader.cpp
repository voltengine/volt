#include <volt/pch.hpp>
#include <volt/video/vk12/shader.hpp>

#include <volt/error.hpp>

namespace volt::video::vk12 {

shader::shader(VkDevice vk_device, const std::vector<
		uint8_t> &bytecode) : vk_device(vk_device) {
	VkShaderModuleCreateInfo info{};

	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = bytecode.size();
	info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

	// vkCreateShaderModule makes its own copy of bytecode
	VOLT_ASSERT(vkCreateShaderModule(vk_device, &info, nullptr,
			&vk_module) == VK_SUCCESS, "Failed to create shader module.");
}

shader::~shader() {
	vkDestroyShaderModule(vk_device, vk_module, nullptr);
}

}
