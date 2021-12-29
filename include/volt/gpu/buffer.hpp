#pragma once

#include "../macros.hpp"

#include <memory>

#include "resource.hpp"

namespace volt::gpu {

class device;

using buffer_features = uint8_t;
namespace buffer_feature {
	constexpr buffer_features
			dynamic_read  = 1 << 0, // Will be repeatedly read by host
			dynamic_write = 1 << 1, // Will be repeatedly written by host

			storage       = 1 << 2, // Can be used as storage buffer (SSBO)
			constant      = 1 << 3, // Can be used as constant buffer (UBO)
			index         = 1 << 4, // Can be used as index buffer (EBO)
			vertex        = 1 << 5; // Can be used as vertex buffer (VBO)
};

class buffer : public gpu::resource {
public:
	virtual ~buffer() = default;

	VOLT_API buffer_features features() const;

	VOLT_API size_t size() const;
	
protected:
	buffer_features _features;
	size_t _size;

	buffer(std::shared_ptr<gpu::device> &&device,
			buffer_features features, size_t size);
};

}
