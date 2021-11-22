#include <volt/gpu/d3d12/instance.hpp>

#include <volt/gpu/d3d12/adapter.hpp>
#include <volt/gpu/d3d12/d3d12.hpp>
#include <volt/error.hpp>

namespace volt::gpu::d3d12 {

instance::instance() {
	UINT flags = 0;

#ifdef VOLT_GPU_DEBUG
	ID3D12Debug *debug;
	VOLT_D3D12_CHECK(D3D12GetDebugInterface(IID_PPV_ARGS(&debug)),
			"Failed to get debug interface.");

	VOLT_D3D12_CHECK(debug->QueryInterface(IID_PPV_ARGS(&this->debug)),
			"Failed to query factory debug interface.");
	debug->Release();

	this->debug->EnableDebugLayer();
	this->debug->SetEnableGPUBasedValidation(true);

	flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	VOLT_D3D12_CHECK(CreateDXGIFactory2(flags, IID_PPV_ARGS(&factory)),
			"Failed to create factory.");
}

instance::~instance() {
	factory->Release();
#ifdef VOLT_GPU_DEBUG
	debug->Release();
#endif
}

std::vector<std::shared_ptr<gpu::adapter>> instance::enumerate_adapters() {
	std::vector<std::shared_ptr<gpu::adapter>> adapters;

	IDXGIAdapter1 *adapter;
	for (UINT index = 0; factory->EnumAdapters1(index,
			&adapter) != DXGI_ERROR_NOT_FOUND; index++) {
		adapters.emplace_back(new d3d12::adapter(shared_from_this(), adapter));
	}

	// Sort adapters, so one with the largest amount of memory will end up at index 0
	std::sort(
		adapters.begin(),
		adapters.end(),
		[](auto &a1, auto &a2) {
			return a1->dedicated_video_memory() > a2->dedicated_video_memory();
		}
	);
	
	return adapters;
}

}
