#pragma once

#include <volt/pch.hpp>

#include <volt/video/enums.hpp>
#include <volt/video/texture_view.hpp>

namespace volt::video::vk12 {

class texture_view : public video::texture_view {
public:
	VkImageView image_view;

	texture_view(
			std::shared_ptr<video::texture> &&texture,
			video::texture_view_type type,
			video::texture_view_aspects aspects);
	
	~texture_view();
};

}
