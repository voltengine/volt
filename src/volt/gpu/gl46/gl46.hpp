#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/gpu.hpp>

namespace volt::gpu::gl46 {

constexpr uint32_t graph_count = 2;

}

#define VOLT_GL46_CHECK(expression, message)\
{\
	::VkResult _volt_vk12_check_result = expression;\
	VOLT_ASSERT(_volt_vk12_check_result == VK_SUCCESS, message + ('\n' + ::volt::gpu::vk12::result_messages[_volt_vk12_check_result]))\
}

#ifdef VOLT_GPU_DEBUG
	#define VOLT_GL46_DEBUG_CHECK(expression, message) VOLT_GL46_CHECK(expression, message)
#else
	#define VOLT_GL46_DEBUG_CHECK(expression, message) expression;
#endif
