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

protected:
	VOLT_API instance() = default;
};

}
