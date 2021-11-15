#include <volt/pch.hpp>
#include <volt/video/vk12/vk12.hpp>

#include <volt/log.hpp>

namespace volt::video::vk12 {

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

std::map<video::resource::type, VmaMemoryUsage> vma_memory_usages{
	{ video::resource::type::internal, VMA_MEMORY_USAGE_GPU_ONLY },
	{ video::resource::type::upload, VMA_MEMORY_USAGE_CPU_ONLY },
	{ video::resource::type::write, VMA_MEMORY_USAGE_CPU_TO_GPU },
	{ video::resource::type::read, VMA_MEMORY_USAGE_GPU_TO_CPU }
};

std::map<video::texture::format, VkFormat> texture_formats{
	{ video::texture::format::r8,    VK_FORMAT_R8_UNORM },
	{ video::texture::format::rg8,   VK_FORMAT_R8G8_UNORM },
	{ video::texture::format::rgb8,  VK_FORMAT_R8G8B8_UNORM },
	{ video::texture::format::rgba8, VK_FORMAT_R8G8B8A8_UNORM },

	{ video::texture::format::r16,    VK_FORMAT_R16_UNORM },
	{ video::texture::format::rg16,   VK_FORMAT_R16G16_UNORM },
	{ video::texture::format::rgb16,  VK_FORMAT_R16G16B16_UNORM },
	{ video::texture::format::rgba16, VK_FORMAT_R16G16B16A16_UNORM },

	{ video::texture::format::r16f,    VK_FORMAT_R16_SFLOAT },
	{ video::texture::format::rg16f,   VK_FORMAT_R16G16_SFLOAT },
	{ video::texture::format::rgb16f,  VK_FORMAT_R16G16B16_SFLOAT },
	{ video::texture::format::rgba16f, VK_FORMAT_R16G16B16A16_SFLOAT },

	{ video::texture::format::r32f,    VK_FORMAT_R32_SFLOAT },
	{ video::texture::format::rg32f,   VK_FORMAT_R32G32_SFLOAT },
	{ video::texture::format::rgb32f,  VK_FORMAT_R32G32B32_SFLOAT },
	{ video::texture::format::rgba32f, VK_FORMAT_R32G32B32A32_SFLOAT },

	{ video::texture::format::d16,  VK_FORMAT_D16_UNORM },
	{ video::texture::format::d32f, VK_FORMAT_D32_SFLOAT },
	{ video::texture::format::s8,   VK_FORMAT_S8_UINT },

	{ video::texture::format::d16_s8,  VK_FORMAT_D16_UNORM_S8_UINT },
	{ video::texture::format::d32f_s8, VK_FORMAT_D32_SFLOAT_S8_UINT },
	{ video::texture::format::d24_s8,  VK_FORMAT_D24_UNORM_S8_UINT },

	{ video::texture::format::bc1,           VK_FORMAT_BC1_RGB_UNORM_BLOCK },
	{ video::texture::format::bc1_srgb,      VK_FORMAT_BC1_RGB_SRGB_BLOCK },
	{ video::texture::format::bc1_srgb_rgba, VK_FORMAT_BC1_RGBA_SRGB_BLOCK },

	{ video::texture::format::bc4,        VK_FORMAT_BC4_UNORM_BLOCK },
	{ video::texture::format::bc5_signed, VK_FORMAT_BC5_SNORM_BLOCK },
	{ video::texture::format::bc6,        VK_FORMAT_BC6H_UFLOAT_BLOCK },
	{ video::texture::format::bc7_srgb,   VK_FORMAT_BC7_SRGB_BLOCK }
};

}
