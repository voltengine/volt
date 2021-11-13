#pragma once

#include "../../macros.hpp"

#include <d3d12.h>

#include "../adapter.hpp"

namespace volt::video::d3d12::_internal {

class pipeline : public video::pipeline {
public:
	VOLT_API pipeline(ID3D12Device *d3d_device);

	VOLT_API ~pipeline();

private:
	ID3D12Device *d3d_device;
	ID3D12PipelineState *d3d_pipeline;
};

}
