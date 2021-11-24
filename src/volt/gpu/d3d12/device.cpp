// #include <volt/gpu/d3d12/device.hpp>

// #include <volt/math/math.hpp>
// #include <volt/gpu/d3d12/adapter.hpp>
// #include <volt/gpu/d3d12/d3d12.hpp>
// #include <volt/gpu/d3d12/fence.hpp>
// #include <volt/gpu/d3d12/queue.hpp>
// #include <volt/gpu/d3d12/swapchain.hpp>
// #include <volt/error.hpp>

// namespace volt::gpu::d3d12 {

// device::device(std::shared_ptr<gpu::adapter> &&adapter) : gpu::device(std::move(adapter)) {
// 	auto d3d_adapter = static_cast<d3d12::adapter *>(this->adapter.get())->d3d_adapter;
	
// 	VOLT_D3D12_CHECK(D3D12CreateDevice(d3d_adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&d3d_device)),
// 			"Failed to create device.")
// #ifdef VOLT_GPU_DEBUG
// 	VOLT_D3D12_CHECK(d3d_device->QueryInterface(&debug_device),
// 			"Failed to query device debug interface.")

// 	VOLT_D3D12_CHECK(d3d_device->QueryInterface(IID_PPV_ARGS(&info_queue)),
// 		"Failed to query device info queue interface.");
// #endif
	
// 	D3D12MA::ALLOCATOR_DESC allocator_desc{};
// 	allocator_desc.pDevice = d3d_device;
// 	allocator_desc.pAdapter = d3d_adapter;
	
// 	VOLT_D3D12_CHECK(D3D12MA::CreateAllocator(&allocator_desc, &allocator),
// 			"Failed to create allocator.")
// }

// device::~device() {
// #ifdef VOLT_GPU_DEBUG
// 	info_queue->Release();
// 	debug_device->Release();
// #endif
// 	d3d_device->Release();
// }

// void device::wait() {
// 	// TODO: Wait for each queue
// }

// std::shared_ptr<gpu::buffer> device::create_buffer(
// 		gpu::memory_type memory_type,
// 		gpu::command_types sync_queues,
// 		gpu::buffer_features features,
// 		size_t size) {
// 	return nullptr;
// }

// std::shared_ptr<gpu::fence> device::create_fence(uint64_t initial_value) {
// 	return std::shared_ptr<gpu::fence>(new d3d12::fence(
// 			shared_from_this(), initial_value));
// }

// std::shared_ptr<gpu::swapchain> device::create_swapchain(std::shared_ptr<os::window> window) {
// 	return std::shared_ptr<gpu::swapchain>(new d3d12::swapchain(
// 			shared_from_this(), std::move(window)));
// }

// std::shared_ptr<gpu::texture> device::create_texture(
// 		gpu::memory_type memory_type,
// 		gpu::command_types sync_queues,
// 		gpu::texture_features features,
// 		size_t size, uint32_t levels,
// 		gpu::texture_format format) {
// 	return nullptr;
// }

// std::shared_ptr<gpu::texture> device::create_texture(
// 		gpu::memory_type memory_type,
// 		gpu::command_types sync_queues,
// 		gpu::texture_features features,
// 		math::uvec2 size, uint32_t levels,
// 		gpu::texture_format format) {
// 	return nullptr;
// }

// std::shared_ptr<gpu::texture> device::create_texture(
// 		gpu::memory_type memory_type,
// 		gpu::command_types sync_queues,
// 		gpu::texture_features features,
// 		math::uvec3 size, uint32_t levels,
// 		gpu::texture_format format) {
// 	return nullptr;
// }

// // std::shared_ptr<gpu::pipeline> device::create_pipeline() {
// // 	return nullptr; // TODO: Implement
// // }

// std::shared_ptr<gpu::rasterization_queue> device::new_rasterization_queue() {
// 	return std::shared_ptr<gpu::rasterization_queue>(
// 			new d3d12::rasterization_queue(shared_from_this()));
// }

// std::shared_ptr<gpu::compute_queue> device::new_compute_queue() {
// 	return std::shared_ptr<gpu::compute_queue>(
// 			new d3d12::compute_queue(shared_from_this()));
// }

// std::shared_ptr<gpu::copy_queue> device::new_copy_queue() {
// 	return std::shared_ptr<gpu::copy_queue>(
// 			new d3d12::copy_queue(shared_from_this()));
// }

// }
