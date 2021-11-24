// #include <volt/gpu/d3d12/swapchain.hpp>

// #include <volt/gpu/d3d12/adapter.hpp>
// #include <volt/gpu/d3d12/device.hpp>
// #include <volt/gpu/d3d12/instance.hpp>
// // #include <volt/gpu/d3d12/texture.hpp>
// #include <volt/gpu/d3d12/d3d12.hpp>
// #include <volt/gpu/d3d12/queue.hpp>
// #include <volt/error.hpp>

// static void frame_resize_callback() {
	
// }

// namespace volt::gpu::d3d12 {

// using namespace math;

// swapchain::swapchain(std::shared_ptr<gpu::device> &&device, std::shared_ptr<os::window> &&window)
// 		: gpu::swapchain(std::move(device), std::move(window)) {
// 	auto &_device = *static_cast<d3d12::device *>(this->device.get());
// 	auto &adapter = *static_cast<d3d12::adapter *>(_device.get_adapter().get());
// 	auto &instance = *static_cast<d3d12::instance *>(adapter.get_instance().get());

// 	queue = this->device->get_rasterization_queue();
// 	this->window->_construct_swapchain();
// 	uvec2 size = this->window->get_frame_size();
// 	HWND hwnd = glfwGetWin32Window(this->window->_glfw_window);

// 	DXGI_SWAP_CHAIN_DESC1 swapchain_desc{};
//     swapchain_desc.BufferCount = 3; // Triple buffering
//     swapchain_desc.Width = size.x;
//     swapchain_desc.Height = size.y;
//     swapchain_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//     swapchain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//     swapchain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // D3D12 requires flipping + we don't need backbuffer contents to persist
//     swapchain_desc.SampleDesc.Count = 1; // No MSAA

//     VOLT_D3D12_CHECK(instance.factory->CreateSwapChainForHwnd(
// 			static_cast<d3d12::rasterization_queue *>(queue.get())->command_queue,
// 			hwnd, &swapchain_desc, nullptr, nullptr,
// 			&d3d_swapchain), "Failed to create swapchain.")

// 	this->window->_on_frame_resize(frame_resize_callback);
// }

// swapchain::~swapchain() {
// 	auto &_device = *static_cast<d3d12::device *>(device.get());
// 	auto &_instance = *static_cast<d3d12::instance *>(device->get_adapter()->get_instance().get());

// 	// Destroy swapchain
// 	d3d_swapchain->Release();

// 	window->_destruct_swapchain();
// }

// // std::vector<const std::shared_ptr<gpu::texture>> &surface::get_frames() {
	
// // }

// }
