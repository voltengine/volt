#pragma once

#include "../../macros.hpp"

#include <d3d12.h>

#include "../shader.hpp"

namespace volt::video::d3d12::_internal {

class shader : public video::shader {
public:
	VOLT_API shader(ID3D12Device *d3d_device, const std::vector<uint8_t> &bytecode);

	VOLT_API D3D12_SHADER_BYTECODE get_d3d_bytecode();

private:
	ID3D12Device *d3d_device;
	D3D12_SHADER_BYTECODE d3d_bytecode;
	std::vector<uint8_t> bytecode;
};

}