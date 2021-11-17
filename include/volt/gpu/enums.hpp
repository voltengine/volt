#pragma once

#include "../error.hpp"

namespace volt::gpu {

enum class command_type {
	graphics, compute, copy // Types of queues, pools and routines
};

using sync_queues = uint32_t;
namespace sync_queue {
	constexpr sync_queues
			none     = 0,      // Dummy value that means "empty mask"
			graphics = 1 << 0,
			compute  = 1 << 1,
			copy     = 1 << 2;
}

enum class resource_type {
	internal, // Stored in local device memory, very fast access from GPU
	staging,  // For uploading data to staging buffer
	write,    // For streaming data into GPU, i.e. using a constant buffer; Requires resource::write(...) after access and before unmapping
	read      // For reading output data from GPU; Requires resource::read(...) after mapping and before access 
};

using buffer_features = uint32_t;
namespace buffer_feature {
	constexpr buffer_features
			source      = 1 << 0, // Source for copy operations 
			destination = 1 << 1, // Destination for copy operations
			constant    = 1 << 2, // Can be used as constant buffer / UBO
			storage     = 1 << 3, // Can be used as storage buffer / SSBO
			index       = 1 << 4, // Can be used as index buffer
			vertex      = 1 << 5; // Can be used as vertex buffer / VBO
}

enum class buffer_view_type {
	constant, storage
};

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
			source                   = 1 << 0, // Source for copy operations 
			destination              = 1 << 1, // Destination for copy operations
			sampler                  = 1 << 2, // Can be used as sampler
			storage                  = 1 << 3, // Can be used as storage texture
			color_attachment         = 1 << 4, // Can be used as color attachment
			depth_stencil_attachment = 1 << 5; // Can be used as depth-stencil attachment
}

enum class texture_layout {
	undefined, // Unknown
	universal, // Slow, but supports all operations

	source,      // Source for copy operations 
	destination, // Destination for copy operations

	color_attachment,         // Allow IO as render pass color input attachment 
	depth_stencil_attachment, // Allow output as render pass color input attachment 

	depth_stencil_read_only, // Use as input depth/stencil attachment or sampler
	sampler                  // Use as sampler
};

enum class texture_view_type {
	tex1d, tex1d_array, // 1D array view can access 2D texture
	tex2d, tex2d_array, // 2D array view can access 3D texture
	tex3d,
	cube, cube_array    // Cube view can access layered 2D texture; Cube array view can access layered 3D texture
};

using texture_view_aspects = uint32_t;
namespace texture_view_aspect {
	constexpr texture_view_aspects
			color   = 1 << 0,
			depth   = 1 << 1,
			stencil = 1 << 2;
}

enum class shader_stage {
	vertex,
	tesselation_control,
	tesselation_evaluation, // Must appear with tesselation_control
	geometry,
	pixel,
	compute
};

enum class binding_type {
	constant_buffer,
	storage_buffer,
	sampler,
	static_sampler
};

using binding_stages = uint32_t;
namespace binding_stage {
	constexpr binding_stages
			vertex                 = 1 << 0,
			tesselation_control    = 1 << 1,
			tesselation_evaluation = 1 << 2,
			geometry               = 1 << 3,
			pixel                  = 1 << 4,
			compute                = 1 << 5,
			all                    = 0b111111;
}

enum class pipeline_type {
	graphics, compute
};

enum class pipeline_stage { // Used for barriers
	begin,//...
};

}
