#pragma once

#include "../macros.hpp"

#include <memory>

namespace volt::video {

class texture;

class texture_view {
public:
	enum class type {
		tex1d, tex1d_array,
		tex2d, tex2d_array,
		tex3d,
		cube, cube_array 
	};

	using aspects = uint32_t;
	struct aspect {
		static constexpr aspects
				color   = 1 << 0,
				depth   = 1 << 1,
				stencil = 1 << 2;
	};

	const std::shared_ptr<video::texture> &get_texture() {
		return texture;
	}

protected:
	std::shared_ptr<video::texture> texture;

	texture_view(std::shared_ptr<video::texture> &&texture)
			: texture(std::move(texture)) {}
};

}
