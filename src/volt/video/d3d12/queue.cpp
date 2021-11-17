#include <volt/video/d3d12/queue.hpp>

// #include <volt/video/d3d12/pool.hpp>
#include <volt/video/d3d12/d3d12.hpp>
#include <volt/error.hpp>

namespace volt::video::d3d12 {

template<command_type T>
queue<T>::queue(std::shared_ptr<video::device> &&device)
		: video::queue<T>(std::move(device)) {
	ID3D12Device *d3d_device = static_cast<d3d12::device>(device.get())->d3d_device;

	D3D12_COMMAND_QUEUE_DESC queue_desc{};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	if constexpr (T == command_type::graphics)
		queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	else if constexpr (T == command_type::compute)
		queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;
	else
		queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	VOLT_D3D12_CHECK(d3d_device->CreateCommandQueue(&queue_desc,
			IID_PPV_ARGS(&command_queue)), "Failed to create command queue.")

	wait_fence = this->device->create_fence();
}

template<command_type T>
queue<T>::~queue() {
	wait();
	command_queue->Release();
}

template<command_type T>
std::shared_ptr<video::pool<T>> queue<T>::create_pool() {
	// return std::shared_ptr<video::pool<T>>(
	// 		new d3d12::pool<T>(queue<T>::shared_from_this()));
	return nullptr;
}

template<command_type T>
void queue<T>::wait() {
	signal(wait_fence, ++wait_counter); // Set wait_fence to (counter + 1) after GPU is done
	wait_fence->wait(wait_counter); // Wait for wait_fence until it reaches (counter + 1)
}

template<command_type T>
void queue<T>::wait(const std::shared_ptr<video::fence> &fence, uint64_t value) {

}

template<command_type T>
void queue<T>::execute(const std::shared_ptr<video::routine<T>> &routine) {

}

template<command_type T>
void queue<T>::signal(const std::shared_ptr<video::fence> &fence, uint64_t value) {

}

template<command_type T>
void queue<T>::submit() {

}

template<command_type T>
void queue<T>::wait() {
	
}

template class queue<command_type::graphics>;
template class queue<command_type::compute>;
template class queue<command_type::copy>;

}
