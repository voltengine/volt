#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::video {

class texture;

class texture_view {
public:
	const std::shared_ptr<video::texture> &get_texture() {
		return texture;
	}

protected:
	std::shared_ptr<video::texture> texture;

	texture_view(std::shared_ptr<video::texture> &&texture)
			: texture(std::move(texture)) {}
};

}
