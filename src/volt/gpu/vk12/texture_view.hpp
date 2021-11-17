#pragma once

#include <volt/pch.hpp>

#include <volt/gpu/enums.hpp>
#include <volt/gpu/texture_view.hpp>

namespace volt::gpu::vk12 {

class texture_view : public gpu::texture_view {
public:
	VkImageView image_view;

	texture_view(
			std::shared_ptr<gpu::texture> &&texture,
			gpu::texture_view_type type,
			gpu::texture_view_aspects aspects);
	
	~texture_view();
};

}
