#include <volt/pch.hpp>
#include <volt/video/vk12/vk12.hpp>

#include <volt/log.hpp>

namespace volt::video::vk12::_internal {

// More device extensions can be added here
std::vector<const char *> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<const char *> &validation_layers() {
	static std::vector<const char *> validation_layers = []() {
		// More validation layers can be added here
		std::vector<const char *> validation_layers{
			"VK_LAYER_KHRONOS_validation"
		};

		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
		std::vector<VkLayerProperties> supported_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, supported_layers.data());

		auto it = validation_layers.begin();
		while (it != validation_layers.end()) {
			const char *layer_name = *it;

			if (std::find_if(
				supported_layers.begin(),
				supported_layers.end(),
				[layer_name](const VkLayerProperties &properties) {
					return std::strcmp(layer_name, properties.layerName) == 0;
				}
			) != supported_layers.end()) {
				it++;
				continue;
			}

			it = validation_layers.erase(it);
			VOLT_LOG_WARNING(std::string(layer_name) + " validation layer is unsupported.")
		}

		return validation_layers;
	}();

	return validation_layers;
}

}
