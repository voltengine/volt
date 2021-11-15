#pragma once

#include <volt/macros.hpp>

#include <volt/pch.hpp>

#include <volt/video/texture_view.hpp>

namespace volt::video::vk12 {

class texture_view : public video::texture_view {
public:
	VkImageView image_view;

	VOLT_API texture_view(
			std::shared_ptr<video::texture> &&texture,
			video::texture_view::type type,
			video::texture_view::aspects aspects);
	
	VOLT_API ~texture_view();
};

}
