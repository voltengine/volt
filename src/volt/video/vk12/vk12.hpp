#pragma once

#include <volt/macros.hpp>

#include <vector>

#include <volt/video/video.hpp>

namespace volt::video::vk12 {

VOLT_API extern std::vector<const char *> device_extensions;

VOLT_API const std::vector<const char *> &validation_layers();

extern std::map<video::resource::type, VmaMemoryUsage> vma_memory_usages;

extern std::map<video::texture::format, VkFormat> texture_formats;

}

#include <volt/video/vk12/adapter.hpp>
#include <volt/video/vk12/device.hpp>
#include <volt/video/vk12/instance.hpp>
#include <volt/video/vk12/surface.hpp>
