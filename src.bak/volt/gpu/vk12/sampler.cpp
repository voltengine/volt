#include <volt/gpu/vk12/sampler.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/math/math.hpp>

namespace volt::gpu::vk12 {

sampler::sampler(std::shared_ptr<gpu::device> &&device, sampler_filter filter, bool blur, float anisotropy)
		: gpu::sampler(std::move(device), filter, blur, anisotropy) {}

void sampler::create() {
	auto &vk_device = static_cast<vk12::device *>(_device.get())->vk_device;
	auto &vk12_adapter = *static_cast<vk12::adapter *>(_device->adapter().get());

	_anisotropy = math::min(_anisotropy, vk12_adapter.properties.limits.maxSamplerAnisotropy);

	VkSamplerCreateInfo sampler_info{};
	sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_info.magFilter = _blur ? VK_FILTER_LINEAR : VK_FILTER_NEAREST;
	sampler_info.minFilter = _filter == sampler_filter::nearest ? VK_FILTER_NEAREST : VK_FILTER_LINEAR;
	sampler_info.mipmapMode = _filter == sampler_filter::trilinear ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
	sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_info.anisotropyEnable = _anisotropy != 0;
	sampler_info.maxAnisotropy = _anisotropy;
	sampler_info.maxLod = VK_LOD_CLAMP_NONE;

	VOLT_VK12_CHECK(vkCreateSampler(vk_device, &sampler_info, nullptr, &vk_sampler),
			"Failed to create sampler.")
}

void sampler::destroy() {
	auto &vk_device = static_cast<vk12::device *>(_device.get())->vk_device;
	vkDestroySampler(vk_device, vk_sampler, nullptr);
}

}
