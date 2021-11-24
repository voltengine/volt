#include <volt/gpu/vk12/shader.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

shader::shader(std::shared_ptr<gpu::device> &&device, const std::vector<uint8_t> &bytecode)
		: gpu::shader(std::move(device)) {
	VkDevice vk_device = static_cast<vk12::device *>(this->device.get())->vk_device;

	VkShaderModuleCreateInfo info{};
	info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	info.codeSize = bytecode.size();
	info.pCode = reinterpret_cast<const uint32_t*>(bytecode.data());

	// vkCreateShaderModule makes its own copy of bytecode
	VOLT_VK12_CHECK(vkCreateShaderModule(vk_device, &info, nullptr, &shader_module),
			"Failed to create shader module.")
}

shader::~shader() {
	VkDevice vk_device = static_cast<vk12::device *>(device.get())->vk_device;
	vkDestroyShaderModule(vk_device, shader_module, nullptr);
}

}
