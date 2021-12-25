#include <volt/gpu/gl46/adapter.hpp>

namespace volt::gpu::gl46 {

adapter::adapter(std::shared_ptr<gpu::instance> &&instance)
		: gpu::adapter(std::move(instance)) {

}

std::string adapter::vendor() {
	return reinterpret_cast<const char *>(glGetString(GL_VENDOR));
}

std::string adapter::name() {
	return reinterpret_cast<const char *>(glGetString(GL_RENDERER));
}

uint64_t adapter::total_memory() {
	std::string vendor = this->vendor();
	std::transform(vendor.begin(), vendor.end(), vendor.begin(), ::toupper);

	int32_t memory = 0;

	if (vendor.find("NVIDIA") != std::string::npos)
		glGetIntegerv(GL_GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &memory);
	else if (vendor.find("AMD") != std::string::npos || vendor.find("ATI") != std::string::npos)
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &memory); // TODO Fix; This actually returns the amount of free memory, not the total

	return memory * 1024Ui64;
}

uint64_t adapter::free_memory() {
	std::string vendor = this->vendor();
	std::transform(vendor.begin(), vendor.end(), vendor.begin(), ::toupper);

	int32_t memory = 0;

	if (vendor.find("NVIDIA") != std::string::npos)
		glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &memory);
	else if (vendor.find("AMD") != std::string::npos || vendor.find("ATI") != std::string::npos)
		glGetIntegerv(GL_TEXTURE_FREE_MEMORY_ATI, &memory);

	return memory * 1024Ui64;
}

std::shared_ptr<gpu::device> adapter::create_device() {
	// return std::make_shared<vk12::device>(shared_from_this());
	return nullptr;
}

}
