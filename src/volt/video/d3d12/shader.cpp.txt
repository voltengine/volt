#include <volt/pch.hpp>
#include <volt/video/d3d12/shader.hpp>

namespace volt::video::d3d12::_internal {

shader::shader(ID3D12Device *d3d_device, const std::
		vector<uint8_t> &bytecode) : d3d_device(d3d_device), bytecode(bytecode) {
	d3d_bytecode.pShaderBytecode = this->bytecode.data();
	d3d_bytecode.BytecodeLength = this->bytecode.size();
}

D3D12_SHADER_BYTECODE shader::get_d3d_bytecode() {
	return d3d_bytecode;
}

}
