#pragma once

#include "../macros.hpp"

#include <memory>

#include "resource.hpp"

namespace volt::gpu {

class buffer : public gpu::_resource {
public:
	// virtual buffer_view create_view();

protected:
	buffer(std::shared_ptr<gpu::device> &&device)
			: gpu::_resource(std::move(device)) {}
};

}
