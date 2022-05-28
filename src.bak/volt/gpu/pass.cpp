#include <volt/pch.hpp>
#include <volt/gpu/pass.hpp>

namespace volt::gpu {

async_pass::async_pass(util::thread_pool &thread_pool)
		: thread_pool(thread_pool) {}

}
