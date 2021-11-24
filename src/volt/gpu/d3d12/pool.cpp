// #include <volt/gpu/d3d12/pool.hpp>

// #include <volt/gpu/d3d12/adapter.hpp>
// #include <volt/gpu/d3d12/device.hpp>
// #include <volt/gpu/d3d12/routine.hpp>
// #include <volt/gpu/d3d12/d3d12.hpp>
// #include <volt/error.hpp>

// namespace volt::gpu::d3d12 {

// template<command_types T>
// pool<T>::pool(std::shared_ptr<gpu::queue<T>> &&queue)
// 		: gpu::pool<T>(std::move(queue)) {
// 	auto d3d_device = static_cast<d3d12::device *>(this->queue->get_device().get())->d3d_device;

// 	VOLT_D3D12_CHECK(d3d_device->CreateCommandAllocator(d3d12::command_list_type<T>,
// 			IID_PPV_ARGS(&allocator)), "Failed to create command allocator.")
// }

// template<command_types T>
// pool<T>::~pool() {
// 	allocator->Release();
// }

// template<command_types T>
// std::shared_ptr<gpu::routine<T>> pool<T>::create_routine() {
// 	return std::shared_ptr<gpu::routine<T>>(new d3d12
// 			::routine<T>(pool<T>::shared_from_this()));
// }

// template<command_types T>
// void pool<T>::reset_routines() {
// 	VOLT_D3D12_CHECK(allocator->Reset(), "Failed to reset command allocator.")
// }

// template class pool<command_type::rasterization>;
// template class pool<command_type::compute>;
// template class pool<command_type::copy>;

// }
