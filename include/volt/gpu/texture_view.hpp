#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::gpu {

class texture;

class texture_view {
public:
	const std::shared_ptr<gpu::texture> &get_texture() {
		return texture;
	}

protected:
	std::shared_ptr<gpu::texture> texture;

	texture_view(std::shared_ptr<gpu::texture> &&texture)
			: texture(std::move(texture)) {}
};

}
