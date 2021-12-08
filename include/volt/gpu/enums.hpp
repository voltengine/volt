#pragma once

#include "../error.hpp"

namespace volt::gpu {

using command_types = uint32_t;
namespace command_type {
	constexpr command_types
			none          = 0,
			rasterization = 1 << 0,
			compute       = 1 << 1,
			copy          = 1 << 2;
}

enum class memory_type {
	internal, // Stored in local device memory, very fast access from GPU
	staging,  // For uploading data to staging buffer
	write,    // For streaming data into GPU, i.e. using a constant buffer; Requires resource::write(...) after access and before unmapping
	read      // For reading output data from GPU; Requires resource::read(...) after mapping and before access 
};

using buffer_features = uint32_t;
namespace buffer_feature {
	constexpr buffer_features
			copy_src = 1 << 0, // Source for copy operations 
			copy_dst = 1 << 1, // Destination for copy operations

			constant = 1 << 2, // Can be used as constant buffer (UBO)
			storage  = 1 << 3, // Can be used as storage buffer (SSBO)
			index    = 1 << 4, // Can be used as index buffer (EBO)
			vertex   = 1 << 5; // Can be used as vertex buffer (VBO)
}

enum class texture_format {
	r8,  rg8,  rgb8,  rgba8,       // 8-bit unsigned normalized
	r16, rg16, rgb16, rgba16,      // 16-bit unsigned normalized

	r16f, rg16f, rgb16f, rgba16f,  // 16-bit floating-point
	r32f, rg32f, rgb32f, rgba32f,  // 32-bit floating-point

	d16,    d32f,        s8,       // Depth unsigned normalized, stencil unsigned
	d16_s8, d32f_s8, d24_s8,       // Depth unsigned normalized + stencil unsigned

	bc1, bc1_srgb, bc1_srgb_rgba,  // Channel-packed PBR, opaque, masked
	bc4, bc5_signed, bc6, bc7_srgb // Grayscale, normal, HDR, transparent
};

using texture_features = uint32_t;
namespace texture_feature {
	constexpr texture_features
			copy_src = 1 << 0, // Source for copy operations 
			copy_dst = 1 << 1, // Destination for copy operations

			sampler    = 1 << 2, // Can be used as sampler
			storage    = 1 << 3,
			attachment = 1 << 4; // Can be used as color attachment
}

enum class texture_type {
	tex1d, tex1d_array, // 1D array view can access layered 1D texture
	tex2d, tex2d_array, // 2D array view can access layered 2D texture
	tex3d,
	cube, cube_array    // Cube view and cube array view can access layered 2D texture;
};

enum class attachment_initializer {
	none, clear, preserve
};

enum class present_mode {
	tear,         // Results in tearing; Has no input lag
	vsync,        // Big input lag + clamps to 30 fps for any framerate below 60
	triple_buffer // Small input lag + Higher energy consumption (unavailable with OpenGL)
};

}
