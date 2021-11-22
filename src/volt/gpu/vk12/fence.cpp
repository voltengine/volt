#include <volt/gpu/vk12/fence.hpp>

#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/vk12.hpp>

namespace volt::gpu::vk12 {

fence::fence(std::shared_ptr<gpu::device> &&device, uint64_t initial_value)
		: gpu::fence(std::move(device)) {
	vk_device = static_cast<vk12::device *>(this->device.get())->vk_device;

	VkSemaphoreTypeCreateInfo type_info{};
	type_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
	type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
	type_info.initialValue = initial_value;

	VkSemaphoreCreateInfo semaphore_info{};
	semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphore_info.pNext = &type_info;

	VOLT_VK12_CHECK(vkCreateSemaphore(vk_device, &semaphore_info, nullptr, &semaphore),
			"Failed to create semaphore.")
}

fence::~fence() {
	vkDestroySemaphore(vk_device, semaphore, nullptr);
}

uint64_t fence::value() {
	uint64_t value;
	VOLT_VK12_DEBUG_CHECK(vkGetSemaphoreCounterValue(vk_device, semaphore, &value),
			"Failed to get semaphore counter value.")
	return value;
}

void fence::signal(uint64_t value) {
	VkSemaphoreSignalInfo signal_info{};
	signal_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
	signal_info.semaphore = semaphore;
	signal_info.value = value;

	VOLT_VK12_DEBUG_CHECK(vkSignalSemaphore(vk_device, &signal_info),
			"Host failed to signal semaphore.")
}

void fence::wait(uint64_t value) {
	VkSemaphoreWaitInfo wait_info{};
	wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
	wait_info.semaphoreCount = 1;
	wait_info.pSemaphores = &semaphore;
	wait_info.pValues = &value;

	VOLT_VK12_DEBUG_CHECK(vkWaitSemaphores(vk_device, &wait_info,
			std::numeric_limits<uint64_t>::max()), "Host failed to wait on semaphore.")
}

}
