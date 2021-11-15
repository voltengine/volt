#include <volt/pch.hpp>
#include <volt/video/video.hpp>

#include <volt/video/vk12/vk12.hpp>

namespace volt::video {

std::shared_ptr<video::instance> create_instance(video::api api) {
	switch (api) {
		default:
		case video::api::vk12:
			return std::make_shared<vk12::instance>();
	}
}

}
