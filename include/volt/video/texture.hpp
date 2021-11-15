#pragma once

#include "../macros.hpp"

#include <memory>

#include "resource.hpp"
#include "texture_view.hpp"

namespace volt::video {

class texture : public resource, public std::enable_shared_from_this<texture> {
public:
	using features = uint32_t;
	struct feature {
		static constexpr features
				source           = 1 << 0,
				destination      = 1 << 1,
				sampler          = 1 << 2,
				storage          = 1 << 3,
				color_attachment = 1 << 4,
				alpha_attachment = 1 << 5;
	};

	enum class format {
		r8,  rg8,  rgb8,  rgba8,  // 8-bit unsigned normalized
		r16, rg16, rgb16, rgba16, // 16-bit unsigned normalized

		r16f, rg16f, rgb16f, rgba16f, // 16-bit floating-point
		r32f, rg32f, rgb32f, rgba32f, // 32-bit floating-point

		d16,    d32f,        s8, // depth unsigned normalized, stencil unsigned
		d16_s8, d32f_s8, d24_s8, // depth unsigned normalized + stencil unsigned

		bc1, bc1_srgb, bc1_srgb_rgba,  // channel-packed PBR, opaque, masked
		bc4, bc5_signed, bc6, bc7_srgb //  grayscale, normal, HDR, transparent
	};

	virtual std::shared_ptr<video::texture_view> create_view(
			video::texture_view::type type,
			video::texture_view::aspects aspects) = 0;

protected:
	texture(std::shared_ptr<video::device> &&device)
			: video::resource(std::move(device)) {}
};

}
