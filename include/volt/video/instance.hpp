#pragma once

#include "../macros.hpp"

#include <memory>
#include <string>
#include <vector>

#include "../math/math.hpp"

namespace volt::video {

class adapter;

class instance : public std::enable_shared_from_this<instance> {
public:
	virtual std::vector<std::shared_ptr<video::adapter>> list_adapters() = 0;

protected:
	VOLT_API instance() = default;
};

}
