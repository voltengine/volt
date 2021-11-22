#pragma once

#include <volt/gpu/enums.hpp>

namespace volt::gpu::d3d12 {

extern std::map<HRESULT, std::string> result_messages;
extern std::map<command_types, D3D12_COMMAND_LIST_TYPE> command_list_types;

template<command_types T>
constexpr D3D12_COMMAND_LIST_TYPE command_list_type;

template<>
constexpr D3D12_COMMAND_LIST_TYPE command_list_type<command_type::rasterization> = D3D12_COMMAND_LIST_TYPE_DIRECT;

template<>
constexpr D3D12_COMMAND_LIST_TYPE command_list_type<command_type::compute> = D3D12_COMMAND_LIST_TYPE_COMPUTE;

template<>
constexpr D3D12_COMMAND_LIST_TYPE command_list_type<command_type::copy> = D3D12_COMMAND_LIST_TYPE_COPY;

extern std::map<memory_type, D3D12_HEAP_TYPE> heap_types;

}

#define VOLT_D3D12_CHECK(expression, message)\
{\
	::HRESULT result = expression;\
	VOLT_ASSERT(result == S_OK, message + ('\n' + ::volt::gpu::d3d12::result_messages[result]))\
}

#ifdef VOLT_GPU_DEBUG
	#define VOLT_D3D12_DEBUG_CHECK(expression, message) VOLT_D3D12_CHECK(expression, message)
#else
	#define VOLT_D3D12_DEBUG_CHECK(expression, message) expression;
#endif
