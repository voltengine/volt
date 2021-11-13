#include <volt/pch.hpp>
#include <volt/video/d3d12/pipeline.hpp>

namespace volt::video::d3d12::_internal {

pipeline::pipeline(ID3D12Device *d3d_device /* , const d3d12::shader &vertex_shader, const d3d12::shader &fragment_shader */ ) : d3d_device(d3d_device) {
	// D3D12_GRAPHICS_PIPELINE_STATE_DESC pipeline_desc{};

	// // 🔣 Input Assembly
	// D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {
	// 	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
	// 			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	// 	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,
	// 			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	// };
	// pipeline_desc.InputLayout = {inputElementDescs, 2};

	// // 🦄 Resources
	// pipeline_desc.pRootSignature = rootSignature;

	// // Attach shaders
	// pipeline_desc.VS = vertex_shader.get_d3d_bytecode();
	// pipeline_desc.PS = fragment_shader.get_d3d_bytecode();

	// // 🟨 Rasterization
	// D3D12_RASTERIZER_DESC rasterizer_desc;
	// rasterizer_desc.FillMode = D3D12_FILL_MODE_SOLID;
	// rasterizer_desc.CullMode = D3D12_CULL_MODE_NONE;
	// rasterizer_desc.FrontCounterClockwise = FALSE;
	// rasterizer_desc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	// rasterizer_desc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	// rasterizer_desc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	// rasterizer_desc.DepthClipEnable = TRUE;
	// rasterizer_desc.MultisampleEnable = FALSE;
	// rasterizer_desc.AntialiasedLineEnable = FALSE;
	// rasterizer_desc.ForcedSampleCount = 0;
	// rasterizer_desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	// pipeline_desc.RasterizerState = rasterizer_desc;
	// pipeline_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// // 🌀 Color/Blend
	// D3D12_BLEND_DESC blendDesc;
	// blendDesc.AlphaToCoverageEnable = FALSE;
	// blendDesc.IndependentBlendEnable = FALSE;
	// const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc = {
	// 	FALSE,
	// 	FALSE,
	// 	D3D12_BLEND_ONE,
	// 	D3D12_BLEND_ZERO,
	// 	D3D12_BLEND_OP_ADD,
	// 	D3D12_BLEND_ONE,
	// 	D3D12_BLEND_ZERO,
	// 	D3D12_BLEND_OP_ADD,
	// 	D3D12_LOGIC_OP_NOOP,
	// 	D3D12_COLOR_WRITE_ENABLE_ALL,
	// };
	// for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
	// 	blendDesc.RenderTarget[i] = defaultRenderTargetBlendDesc;
	// pipeline_desc.BlendState = blendDesc;

	// // 🌑 Depth/Stencil State
	// pipeline_desc.DepthStencilState.DepthEnable = FALSE;
	// pipeline_desc.DepthStencilState.StencilEnable = FALSE;
	// pipeline_desc.SampleMask = UINT_MAX;

	// // 🖼️ Output
	// pipeline_desc.NumRenderTargets = 1;
	// pipeline_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	// pipeline_desc.SampleDesc.Count = 1;

	// // 🌟 Create the raster pipeline state
	// VOLT_ASSERT(device->CreateGraphicsPipelineState(&pipeline_desc, IID_PPV_ARGS(
	// 		&d3d_pipeline)) == S_OK, "Failed to create graphics pipeline state.")
}

pipeline::~pipeline() {}

}
