#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/pipeline.hpp>

namespace volt::gpu::d3d12::_internal {

class pipeline : public gpu::pipeline {
public:
	pipeline(ID3D12Device *d3d_device);

	~pipeline();

private:
	ID3D12Device *d3d_device;
	ID3D12PipelineState *d3d_pipeline;
};

}
