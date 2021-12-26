#include <volt/pch.hpp>
#include <volt/gpu/pass.hpp>

namespace volt::gpu {

async_pass_context::async_pass_context(util::thread_pool &thread_pool)
		: thread_pool(thread_pool) {}

}
