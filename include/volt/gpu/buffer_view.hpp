#pragma once

#include "../macros.hpp"

#include <memory>

#include "buffer.hpp"

namespace volt::gpu {

class buffer_view {
protected:
	std::shared_ptr<gpu::buffer> buffer;

	VOLT_API buffer_view(std::shared_ptr<gpu::buffer> &&buffer);
};

}
