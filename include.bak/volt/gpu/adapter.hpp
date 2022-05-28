#pragma once

#include "../macros.hpp"

#include <cstdint>
#include <memory>
#include <string>

#include "instance.hpp"

namespace volt::gpu {

class device;

class adapter : public std::enable_shared_from_this<adapter> {
public:
	virtual ~adapter() = default;

	virtual std::string vendor() = 0;

	virtual std::string name() = 0;

	virtual uint64_t total_memory() = 0;

	virtual uint64_t free_memory() = 0;

	virtual std::shared_ptr<gpu::device> create_device() = 0;

	VOLT_API const std::shared_ptr<gpu::instance> &instance() const;

protected:
	std::shared_ptr<gpu::instance> _instance;

	adapter(std::shared_ptr<gpu::instance> &&instance);
};

}
