#include <volt/pch.hpp>
#include <volt/video/d3d12/instance.hpp>

#include <volt/video/d3d12/adapter.hpp>
#include <volt/video/d3d12/window.hpp>
#include <volt/error.hpp>

namespace volt::video::d3d12 {

instance::instance() {
	UINT flags = 0;

#ifndef NDEBUG
	ID3D12Debug *debug;
	VOLT_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))
			== S_OK, "Failed to get debug interface.");

	VOLT_ASSERT(debug->QueryInterface(IID_PPV_ARGS(&d3d_debug))
			== S_OK, "Failed to query interface.");
	debug->Release();

	d3d_debug->EnableDebugLayer();
	d3d_debug->SetEnableGPUBasedValidation(true);

	flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	VOLT_ASSERT(CreateDXGIFactory2(flags, IID_PPV_ARGS(&d3d_factory))
			== S_OK, "Failed to create factory.");
}

instance::~instance() {
	d3d_factory->Release();
#ifndef NDEBUG
	d3d_debug->Release();
#endif
}

std::vector<std::shared_ptr<video::adapter>> instance::list_adapters() {
	std::vector<std::shared_ptr<video::adapter>> adapters;

	IDXGIAdapter1 *adapter;
	for (UINT index = 0; d3d_factory->EnumAdapters1(index,
			&adapter) != DXGI_ERROR_NOT_FOUND; index++) {
		adapters.emplace_back(new _internal::adapter(adapter));
	}
	
	return adapters;
}

std::shared_ptr<video::window> instance::create_window(
		std::string title, math::uvec2 size) {
	return std::make_shared<_internal::window>(std::move(title), size);
}

}
