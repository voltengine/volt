#pragma once

#include "../../macros.hpp"

#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_4.h>

#include "../instance.hpp"

namespace volt::video::d3d12 {

class instance : public video::instance {
public:
	VOLT_API instance();

	VOLT_API ~instance();

	VOLT_API virtual std::vector<std::shared_ptr<video::adapter>> list_adapters();

	VOLT_API std::shared_ptr<os::window> create_window(std::string title, math::uvec2 size) override;

private:
	IDXGIFactory4 *d3d_factory;
#ifndef NDEBUG
	ID3D12Debug1 *d3d_debug;
#endif
};

}
