// #include <volt/gpu/d3d12/queue.hpp>

// #include <volt/gpu/d3d12/d3d12.hpp>
// #include <volt/gpu/d3d12/device.hpp>
// #include <volt/gpu/d3d12/fence.hpp>
// #include <volt/gpu/d3d12/pool.hpp>
// #include <volt/error.hpp>

// namespace volt::gpu::d3d12 {

// template<command_types T>
// queue<T>::queue(std::shared_ptr<gpu::device> &&device)
// 		: gpu::queue<T>(std::move(device)) {
// 	ID3D12Device *d3d_device = static_cast<d3d12::device *>(
// 			this->device.get())->d3d_device;

// 	D3D12_COMMAND_QUEUE_DESC queue_desc{};
// 	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
// 	queue_desc.Type = d3d12::command_list_type<T>;

// 	VOLT_D3D12_CHECK(d3d_device->CreateCommandQueue(&queue_desc,
// 			IID_PPV_ARGS(&command_queue)), "Failed to create command queue.")

// 	wait_fence = this->device->create_fence();
// }

// template<command_types T>
// queue<T>::~queue() {
// 	wait();
// 	command_queue->Release();
// }

// template<command_types T>
// std::shared_ptr<gpu::pool<T>> queue<T>::create_pool() {
// 	return std::shared_ptr<gpu::pool<T>>(new d3d12::
// 			pool<T>(queue<T>::shared_from_this()));
// }

// template<command_types T>
// void queue<T>::wait(const std::shared_ptr<gpu::fence> &fence, uint64_t value) {
// 	flush();
// 	VOLT_D3D12_DEBUG_CHECK(command_queue->Wait(static_cast<d3d12::fence *>(fence.get())
// 			->d3d_fence, value), "Queue failed to wait for fence.")
// }

// template<command_types T>
// void queue<T>::execute(const std::shared_ptr<gpu::routine<T>> &routine) {
	
// }

// template<command_types T>
// void queue<T>::signal(const std::shared_ptr<gpu::fence> &fence, uint64_t value) {
// 	flush();
// 	VOLT_D3D12_DEBUG_CHECK(command_queue->Signal(static_cast<d3d12::fence *>(fence.get())
// 			->d3d_fence, value), "Queue failed to signal fence.")
// }

// template<command_types T>
// void queue<T>::flush() {
// 	if (command_lists.size() != 0) {
// 		command_queue->ExecuteCommandLists(command_lists.size(), command_lists.data());
// 		command_lists.clear();
// 	}
// }

// template<command_types T>
// void queue<T>::wait() {
// 	signal(wait_fence, ++wait_counter); // Set wait_fence to (counter + 1) after GPU is done
// 	wait_fence->wait(wait_counter); // Wait for wait_fence until it reaches (counter + 1)
// }

// template class queue<command_type::rasterization>;
// template class queue<command_type::compute>;
// template class queue<command_type::copy>;

// }
