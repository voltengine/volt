#pragma once

#include "../macros.hpp"

#include <memory>
#include <string>
#include <vector>

#include "../math/math.hpp"

namespace volt::gpu {

class adapter;

class instance : public std::enable_shared_from_this<instance> {
public:
	virtual ~instance() = default;

	virtual std::vector<std::shared_ptr<gpu::adapter>> enumerate_adapters() = 0;

	// Usually returns 2, OpenGL always forces 1 frame
	virtual uint32_t graph_count() const = 0;

protected:
	instance() = default;
};

}
