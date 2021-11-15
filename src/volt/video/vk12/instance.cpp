#include <volt/pch.hpp>
#include <volt/video/vk12/instance.hpp>

#include <volt/util/util.hpp>
#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/vk12.hpp>
#include <volt/error.hpp>
#include <volt/log.hpp>

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
		VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
		VkDebugUtilsMessageTypeFlagsEXT message_type,
		const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
		void *user_data) {
	std::string message = "[Vulkan] ";
	message += callback_data->pMessage;

	if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		VOLT_LOG_ERROR(message)
	else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
		VOLT_LOG_WARNING(message)
	else
		VOLT_LOG_INFO(message)
	
	return VK_FALSE;
}

static VkDebugUtilsMessengerCreateInfoEXT make_messenger_info() {
	VkDebugUtilsMessengerCreateInfoEXT messenger_info{};

	messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	messenger_info.pfnUserCallback = debug_callback;

	return messenger_info;
}

namespace volt::video::vk12 {

instance::instance() {
	VOLT_ASSERT(gladLoaderLoadVulkan(nullptr, nullptr, nullptr), "Failed to load Vulkan base symbols.")

	util::version version(VOLT_VERSION);
	auto vk_version = VK_MAKE_VERSION(version.major, version.minor, version.patch);

	VkApplicationInfo app_info{};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = "Volt";
	app_info.applicationVersion = vk_version;
	app_info.pEngineName = "Volt";
	app_info.engineVersion = vk_version;
	app_info.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo instance_info{};
	instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instance_info.pApplicationInfo = &app_info;

	// Attach debug functionality to instance info
#ifndef NDEBUG
		// Attach validation layers
	auto &layers = vk12::validation_layers();
	instance_info.enabledLayerCount = static_cast<uint32_t>(layers.size());
	instance_info.ppEnabledLayerNames = layers.data();

		// We must also attach debug messenger info because it's an extension- blah blah blah
		// We will reuse this info to create the actual messenger after the instance is set up
	VkDebugUtilsMessengerCreateInfoEXT messenger_info = make_messenger_info();
	instance_info.pNext = &messenger_info;
#endif

	// Attach required extensions to instance info
	static std::vector<const char *> extensions = []() {
		uint32_t num_glfw_extensions;
		const char **glfw_extensions = glfwGetRequiredInstanceExtensions(&num_glfw_extensions);

		std::vector<const char *> extensions(glfw_extensions,
				glfw_extensions + num_glfw_extensions);

		// More instance extensions can be added here
	#ifndef NDEBUG
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	#endif

		return extensions;
	}();
	instance_info.ppEnabledExtensionNames = extensions.data();
	instance_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());

	// Initialize instance
	VOLT_ASSERT(vkCreateInstance(&instance_info, nullptr, &vk_instance)
			== VK_SUCCESS, "Failed to create instance.")
	VOLT_ASSERT(gladLoaderLoadVulkan(vk_instance, nullptr, nullptr),
			"Failed to load Vulkan instance symbols.")

	// Initialize debug messenger
#ifndef NDEBUG
	VOLT_ASSERT(vkCreateDebugUtilsMessengerEXT(vk_instance, &messenger_info, nullptr, &vk_messenger)
			== VK_SUCCESS, "Failed to create debug utils messenger.")
#endif
}

instance::~instance() {
#ifndef NDEBUG
	vkDestroyDebugUtilsMessengerEXT(vk_instance, vk_messenger, nullptr);
#endif
	vkDestroyInstance(vk_instance, nullptr);
}

std::vector<std::shared_ptr<video::adapter>> instance::list_adapters() {
	uint32_t num_physical_devices = 0;
	vkEnumeratePhysicalDevices(vk_instance, &num_physical_devices, nullptr);

	if (num_physical_devices == 0)
		return {};

	std::vector<VkPhysicalDevice> physical_devices(num_physical_devices);
	vkEnumeratePhysicalDevices(vk_instance, &num_physical_devices, physical_devices.data());

	// We must create dummy window to determine presentation support
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

	GLFWwindow *glfw_dummy_window = glfwCreateWindow(1280, 720, "", nullptr, nullptr);
	VOLT_ASSERT(glfw_dummy_window, "Failed to create window.")

	VkSurfaceKHR vk_dummy_surface;
	VOLT_ASSERT(glfwCreateWindowSurface(vk_instance, glfw_dummy_window, nullptr, &vk_dummy_surface)
			== VK_SUCCESS, "Failed to create window surface.")

	std::vector<std::shared_ptr<video::adapter>> adapters;
	adapters.reserve(num_physical_devices);
	for (uint32_t i = 0; i < num_physical_devices; i++) {
		VkPhysicalDevice physical_device = physical_devices[i];
		auto adapter = std::make_shared<vk12::adapter>(shared_from_this(), physical_device, vk_dummy_surface);

		// Check queue families availability
		if (adapter->present_family == -1 || adapter->graphics_family == -1 ||
				adapter->compute_family == -1 ||  adapter->transfer_family == -1) {
			continue;
		}

		// Test if there's sufficient amount of queues available
		// TODO: Drob this part and make queues shareable
		auto families = adapter->families;
		if (--families[adapter->present_family].queueCount < 0)
			continue;
		if (--families[adapter->graphics_family].queueCount < 0)
			continue;
		if (--families[adapter->compute_family].queueCount < 0)
			continue;
		if (--families[adapter->transfer_family].queueCount < 0)
			continue;

		// Check device extension support
		if (!std::all_of(
			vk12::device_extensions.begin(),
			vk12::device_extensions.end(),
			[&](const char *extension_name) {
				return std::find_if(
					adapter->supported_extensions.begin(),
					adapter->supported_extensions.end(),
					[&](VkExtensionProperties extension_properties) {
						return std::strcmp(extension_properties.extensionName, extension_name) == 0;
					}
				) != adapter->supported_extensions.end();
			}
		)) {
			continue;
		}

		// Check swapchain support
		if (adapter->surface_formats.empty() || adapter->surface_present_modes.empty())
			continue;

		adapters.push_back(adapter);
	}

	vkDestroySurfaceKHR(vk_instance, vk_dummy_surface, nullptr);
	glfwDestroyWindow(glfw_dummy_window);

	return adapters;
}

}
