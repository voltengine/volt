#include <volt/gpu/vk12/vk12.hpp>

#include <volt/log.hpp>

static uint32_t glad_refcount = 0;
static VkInstance glad_instance = VK_NULL_HANDLE;
static VkPhysicalDevice glad_physical_device = VK_NULL_HANDLE;
static VkDevice glad_device = VK_NULL_HANDLE;

namespace volt::gpu::vk12 {

std::unordered_map<VkResult, std::string> result_messages{
	{ VK_SUCCESS, "Command successfully completed. (VK_SUCCESS)" },
	{ VK_NOT_READY, "A fence or query has not yet completed. (VK_NOT_READY)" },
	{ VK_TIMEOUT, "A wait operation has not completed in the specified time. (VK_TIMEOUT)" },
	{ VK_EVENT_SET, "An event is signaled. (VK_EVENT_SET)" },
	{ VK_EVENT_RESET, "An event is unsignaled. (VK_EVENT_RESET)" },
	{ VK_INCOMPLETE, "A return array was too small for the result. (VK_INCOMPLETE)" },
	{ VK_ERROR_OUT_OF_HOST_MEMORY, "A host memory allocation has failed. (VK_ERROR_OUT_OF_HOST_MEMORY)" },
	{ VK_ERROR_OUT_OF_DEVICE_MEMORY, "A device memory allocation has failed. (VK_ERROR_OUT_OF_DEVICE_MEMORY)" },
	{ VK_ERROR_INITIALIZATION_FAILED, "Initialization of an object could not be completed for implementation-specific reasons. (VK_ERROR_INITIALIZATION_FAILED)" },
	{ VK_ERROR_DEVICE_LOST, "The logical or physical device has been lost. (VK_ERROR_DEVICE_LOST)" },
	{ VK_ERROR_MEMORY_MAP_FAILED, "Mapping of a memory object has failed. (VK_ERROR_MEMORY_MAP_FAILED)" },
	{ VK_ERROR_LAYER_NOT_PRESENT, "A requested layer is not present or could not be loaded. (VK_ERROR_LAYER_NOT_PRESENT)" },
	{ VK_ERROR_EXTENSION_NOT_PRESENT, "A requested extension is not supported. (VK_ERROR_EXTENSION_NOT_PRESENT)" },
	{ VK_ERROR_FEATURE_NOT_PRESENT, "A requested feature is not supported. (VK_ERROR_FEATURE_NOT_PRESENT)" },
	{ VK_ERROR_INCOMPATIBLE_DRIVER, "The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons. (VK_ERROR_INCOMPATIBLE_DRIVER)" },
	{ VK_ERROR_TOO_MANY_OBJECTS, "Too many objects of the type have already been created. (VK_ERROR_TOO_MANY_OBJECTS)" },
	{ VK_ERROR_FORMAT_NOT_SUPPORTED, "A requested format is not supported on this device. (VK_ERROR_FORMAT_NOT_SUPPORTED)" },
	{ VK_ERROR_FRAGMENTED_POOL, "A pool allocation has failed due to fragmentation of the pool’s memory. (VK_ERROR_FRAGMENTED_POOL)" },
	{ VK_ERROR_UNKNOWN, "An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred. (VK_ERROR_UNKNOWN)" },
	{ VK_ERROR_OUT_OF_POOL_MEMORY, "A pool memory allocation has failed. (VK_ERROR_OUT_OF_POOL_MEMORY)" },
	{ VK_ERROR_INVALID_EXTERNAL_HANDLE, "An external handle is not a valid handle of the specified type. (VK_ERROR_INVALID_EXTERNAL_HANDLE)" },
	{ VK_ERROR_FRAGMENTATION, "A descriptor pool creation has failed due to fragmentation. (VK_ERROR_FRAGMENTATION)" },
	{ VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS, "A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid. (VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)" }
};

// More device extensions can be added here
std::vector<const char *> device_extensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

std::unordered_map<gpu::memory_type, VmaMemoryUsage> vma_memory_usages{
	{ gpu::memory_type::internal, VMA_MEMORY_USAGE_GPU_ONLY },
	{ gpu::memory_type::staging,  VMA_MEMORY_USAGE_CPU_ONLY },
	{ gpu::memory_type::write,    VMA_MEMORY_USAGE_CPU_TO_GPU },
	{ gpu::memory_type::read,     VMA_MEMORY_USAGE_GPU_TO_CPU }
};

std::unordered_map<gpu::texture_format, VkFormat> texture_formats{
	{ gpu::texture_format::r8,    VK_FORMAT_R8_UNORM },
	{ gpu::texture_format::rg8,   VK_FORMAT_R8G8_UNORM },
	{ gpu::texture_format::rgb8,  VK_FORMAT_R8G8B8_UNORM },
	{ gpu::texture_format::rgba8, VK_FORMAT_R8G8B8A8_UNORM },

	{ gpu::texture_format::r16,    VK_FORMAT_R16_UNORM },
	{ gpu::texture_format::rg16,   VK_FORMAT_R16G16_UNORM },
	{ gpu::texture_format::rgb16,  VK_FORMAT_R16G16B16_UNORM },
	{ gpu::texture_format::rgba16, VK_FORMAT_R16G16B16A16_UNORM },

	{ gpu::texture_format::r16f,    VK_FORMAT_R16_SFLOAT },
	{ gpu::texture_format::rg16f,   VK_FORMAT_R16G16_SFLOAT },
	{ gpu::texture_format::rgb16f,  VK_FORMAT_R16G16B16_SFLOAT },
	{ gpu::texture_format::rgba16f, VK_FORMAT_R16G16B16A16_SFLOAT },

	{ gpu::texture_format::r32f,    VK_FORMAT_R32_SFLOAT },
	{ gpu::texture_format::rg32f,   VK_FORMAT_R32G32_SFLOAT },
	{ gpu::texture_format::rgb32f,  VK_FORMAT_R32G32B32_SFLOAT },
	{ gpu::texture_format::rgba32f, VK_FORMAT_R32G32B32A32_SFLOAT },

	{ gpu::texture_format::d16,  VK_FORMAT_D16_UNORM },
	{ gpu::texture_format::d32f, VK_FORMAT_D32_SFLOAT },
	{ gpu::texture_format::s8,   VK_FORMAT_S8_UINT },

	{ gpu::texture_format::d16_s8,  VK_FORMAT_D16_UNORM_S8_UINT },
	{ gpu::texture_format::d32f_s8, VK_FORMAT_D32_SFLOAT_S8_UINT },
	{ gpu::texture_format::d24_s8,  VK_FORMAT_D24_UNORM_S8_UINT },

	{ gpu::texture_format::bc1,           VK_FORMAT_BC1_RGB_UNORM_BLOCK },
	{ gpu::texture_format::bc1_srgb,      VK_FORMAT_BC1_RGB_SRGB_BLOCK },
	{ gpu::texture_format::bc1_srgb_rgba, VK_FORMAT_BC1_RGBA_SRGB_BLOCK },

	{ gpu::texture_format::bc4,        VK_FORMAT_BC4_UNORM_BLOCK },
	{ gpu::texture_format::bc5_signed, VK_FORMAT_BC5_SNORM_BLOCK },
	{ gpu::texture_format::bc6,        VK_FORMAT_BC6H_UFLOAT_BLOCK },
	{ gpu::texture_format::bc7_srgb,   VK_FORMAT_BC7_SRGB_BLOCK }
};

void load_glad() {
	if (glad_refcount++ == 0) {
		VOLT_ASSERT(gladLoaderLoadVulkan(nullptr, nullptr, nullptr),
				"Failed to load Vulkan base symbols.")
	}
}

void unload_glad() {
	if (--glad_refcount == 0) {
		gladLoaderUnloadVulkan();
		glad_instance = VK_NULL_HANDLE;
		glad_physical_device = VK_NULL_HANDLE;
		glad_device = VK_NULL_HANDLE;
	}
}

void load_glad_instance(VkInstance instance) {
	if (glad_instance == VK_NULL_HANDLE) {
		glad_instance = instance;
		VOLT_ASSERT(gladLoaderLoadVulkan(glad_instance, nullptr, nullptr),
				"Failed to load Vulkan instance symbols.")
	}
}

void load_glad_physical_device(VkPhysicalDevice physical_device) {
	if (glad_instance == VK_NULL_HANDLE) {
		glad_physical_device = physical_device;
		VOLT_ASSERT(gladLoaderLoadVulkan(glad_instance, glad_physical_device, nullptr),
				"Failed to load Vulkan physical device symbols.")
	}
}

void load_glad_device(VkDevice device) {
	if (glad_instance == VK_NULL_HANDLE) {
		glad_device = device;
		VOLT_ASSERT(gladLoaderLoadVulkan(glad_instance, glad_physical_device, glad_device),
				"Failed to load Vulkan device symbols.")
	}
}

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
			VOLT_LOG_WARNING(std::string(layer_name) + " validation layer is unsupported. (VK_NOT_READY)")
		}

		return validation_layers;
	}();

	return validation_layers;
}

}
