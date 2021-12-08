// #include <volt/gpu/d3d12/routine.hpp>

// #include <volt/gpu/d3d12/device.hpp>
// #include <volt/gpu/d3d12/pool.hpp>
// #include <volt/gpu/d3d12/d3d12.hpp>
// #include <volt/error.hpp>

// namespace volt::gpu::d3d12 {

// template<command_types T>
// routine<T>::routine(std::shared_ptr<gpu::pool<T>> &&pool)
// 		: gpu::routine<T>(std::move(pool)) {
// 	allocator = static_cast<d3d12::pool<T> *>(this->pool.get())->allocator;
// 	auto device = static_cast<d3d12::device *>(this->pool->get_queue()->get_device().get())->d3d_device;

// 	VOLT_D3D12_CHECK(device->CreateCommandList(0, d3d12::command_list_type<T>, allocator,
// 			nullptr, IID_PPV_ARGS(&command_list)), "Failed to create command list.")
// 	end();
// }

// template<command_types T>
// routine<T>::~routine() {
// 	command_list->Release();
// }

// template<command_types T>
// void routine<T>::begin() {
// 	VOLT_D3D12_DEBUG_CHECK(command_list->Reset(allocator, nullptr), "Failed to reset command list.")
// }
	
// template<command_types T>
// void routine<T>::end() {
// 	VOLT_D3D12_DEBUG_CHECK(command_list->Close(), "Failed to close command list.")
// }

// template class routine<command_type::rasterization>;
// template class routine<command_type::compute>;
// template class routine<command_type::copy>;

// }