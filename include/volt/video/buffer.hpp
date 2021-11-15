#pragma once

#include "../macros.hpp"

#include <memory>

#include "resource.hpp"

namespace volt::video {

class buffer : public resource {
public:
	using features = uint32_t;
	struct feature {
		static constexpr features
				source      = 1 << 0,
				destination = 1 << 1,
				constant    = 1 << 2,
				storage     = 1 << 3,
				index       = 1 << 4,
				vertex      = 1 << 5;
	};

	// virtual buffer_view create_view();

protected:
	buffer(std::shared_ptr<video::device> &&device)
			: video::resource(std::move(device)) {}
};

}
