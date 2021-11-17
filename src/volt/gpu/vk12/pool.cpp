#include <volt/gpu/vk12/pool.hpp>

#include <volt/gpu/vk12/adapter.hpp>
#include <volt/gpu/vk12/device.hpp>
#include <volt/gpu/vk12/routine.hpp>
#include <volt/gpu/vk12/vk12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::vk12 {

template<command_type T>
pool<T>::pool(std::shared_ptr<gpu::queue<T>> &&queue)
		: gpu::pool<T>(std::move(queue)) {
	auto &_device = *static_cast<vk12::device *>(this->queue->get_device().get());
	device = _device.vk_device;
	auto &adapter = *static_cast<vk12::adapter *>(_device.get_adapter().get());

	VkCommandPoolCreateInfo pool_info{};
	pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	if constexpr (T == command_type::graphics)
		pool_info.queueFamilyIndex = adapter.graphics_family;
	if constexpr (T == command_type::compute)
		pool_info.queueFamilyIndex = adapter.compute_family;
	if constexpr (T == command_type::copy)
		pool_info.queueFamilyIndex = adapter.transfer_family;

	VOLT_VK12_CHECK(vkCreateCommandPool(device, &pool_info, nullptr, &command_pool),
			"Failed to create command pool.")
}

template<command_type T>
pool<T>::~pool() {
	vkDestroyCommandPool(device, command_pool, nullptr);
}

template<command_type T>
std::shared_ptr<gpu::routine<T>> pool<T>::create_routine() {
	return std::shared_ptr<gpu::routine<T>>(
			new typename vk12::routine<T>(pool<T>::shared_from_this()));
}

template<command_type T>
std::shared_ptr<gpu::subroutine<T>> pool<T>::create_subroutine() {
	return std::shared_ptr<gpu::subroutine<T>>(
			new typename vk12::subroutine<T>(pool<T>::shared_from_this()));
}

template<command_type T>
void pool<T>::reset_routines() {
	VOLT_VK12_DEBUG_CHECK(vkResetCommandPool(device, command_pool, 0),
			"Failed to reset command pool.")
}

template class pool<command_type::graphics>;
template class pool<command_type::compute>;
template class pool<command_type::copy>;

}
