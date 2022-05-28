#pragma once

#include "../macros.hpp"

#include <memory>

#include "../math/math.hpp"
#include "resource.hpp"

namespace volt::gpu {

class device;

using texture_features = uint8_t;
namespace feature {
	constexpr texture_features
			dynamic_read  = 1 << 0, // Will be repeatedly read by host
			dynamic_write = 1 << 1, // Will be repeatedly written by host

			storage    = 1 << 2, // Can be used as storage texture
			sampled    = 1 << 3, // Can be used as sampled texture
			attachment = 1 << 4; // Can be used as attachment
};

enum class texture_format {
	r8,  rg8,  rgb8,  rgba8,       // 8-bit unsigned normalized
	r16, rg16, rgb16, rgba16,      // 16-bit unsigned normalized

	r16f, rg16f, rgb16f, rgba16f,  // 16-bit floating-point
	r32f, rg32f, rgb32f, rgba32f,  // 32-bit floating-point

	d16,    d32f,        s8,       // Depth unsigned normalized, stencil unsigned
	d16_s8, d32f_s8, d24_s8,       // Depth unsigned normalized + stencil unsigned

	bc1, bc1_srgb, bc1_srgb_rgba,  // Channel-packed PBR, opaque, masked
	bc4, bc5_signed, bc6, bc7_srgb, // Grayscale, normal, HDR, transparent
	present
};

class texture : public gpu::resource {
public:
	virtual ~texture() = default;

	VOLT_API texture_features features() const;

	VOLT_API texture_format format() const;

	VOLT_API uint32_t levels() const;

	VOLT_API math::uvec3 size() const;

protected:
	texture_features _features;
	texture_format _format;
	uint32_t _levels;
	math::uvec3 _size;

	texture(std::shared_ptr<gpu::device> &&device,
			texture_features features, texture_format format,
			uint32_t levels, math::uvec3 size);
};

}
