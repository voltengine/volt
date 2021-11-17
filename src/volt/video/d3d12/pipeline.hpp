#pragma once

#include <volt/pch.hpp>

#include <volt/video/pipeline.hpp>

namespace volt::video::d3d12::_internal {

class pipeline : public video::pipeline {
public:
	pipeline(ID3D12Device *d3d_device);

	~pipeline();

private:
	ID3D12Device *d3d_device;
	ID3D12PipelineState *d3d_pipeline;
};

}
