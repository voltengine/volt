#pragma once

namespace volt::video::d3d12 {

extern std::map<HRESULT, std::string> result_messages;

}

#define VOLT_D3D12_CHECK(expression, message)\
{\
	::HRESULT result = expression;\
	VOLT_ASSERT(result == S_OK, message + ('\n' + ::volt::video::d3d12::result_messages[result]))\
}

#ifdef VOLT_VIDEO_DEBUG
	#define VOLT_D3D12_DEBUG_CHECK(expression, message) VOLT_D3D12_CHECK(expression, message)
#else
	#define VOLT_D3D12_DEBUG_CHECK(expression, message) expression;
#endif

#include "adapter.hpp"
#include "device.hpp"
#include "instance.hpp"

