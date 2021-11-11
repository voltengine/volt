#pragma once

#include "../macros.hpp"

#define VOLT_STATIC_BLOCK_CONCATENATE(a, b) a##b
#define VOLT_STATIC_BLOCK_JOIN(a, b) VOLT_STATIC_BLOCK_CONCATENATE(a, b)

#define VOLT_STATIC_BLOCK_IMPL(fun_name, var_name)\
		static void fun_name();\
		static const bool var_name = ([]() { fun_name(); return false; })();\
		static void fun_name()

#define VOLT_STATIC_BLOCK VOLT_STATIC_BLOCK_IMPL(\
		VOLT_STATIC_BLOCK_JOIN(volt_static_block_, __LINE__)_fun,\
		VOLT_STATIC_BLOCK_JOIN(volt_static_block_, __LINE__)_var)
