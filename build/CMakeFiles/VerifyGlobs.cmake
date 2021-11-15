# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.20
cmake_policy(SET CMP0009 NEW)

# TARGET_SOURCES at CMakeLists.txt:137 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/app/*.cpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/app/app.cpp"
  "D:/Library/Source/voltengine/volt/app/pch.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:137 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/app/*.hpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/app/pch.hpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:137 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/app/*.inl")
set(OLD_GLOB
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_HEADERS at CMakeLists.txt:53 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/include/*.hpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/include/volt/assets/asset.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/assets/asset_ref.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/assets/assets.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/config.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/core/assets/text.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/component_storage.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/ecs.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/entity.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/system.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/world.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/error.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/log.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/macros.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/math/mat3.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/math/math.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/math/quat.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/math/vec2.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/math/vec3.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/math/vec4.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/modules.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/os/monitor.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/os/os.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/os/window.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/paths.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/file.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/string.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/util.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/version.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/adapter.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/buffer.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/buffer_view.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/device.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/fence.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/instance.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/pool.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/queue.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/resource.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/routine.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/shader.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/surface.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/texture.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/texture_view.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/video/video.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/volt.hpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:68 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/include/*.inl")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/include/volt/assets/asset_ref.inl"
  "D:/Library/Source/voltengine/volt/include/volt/assets/assets.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/component_storage.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/ecs.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/entity.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/system.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/world.inl"
  "D:/Library/Source/voltengine/volt/include/volt/math/mat3.inl"
  "D:/Library/Source/voltengine/volt/include/volt/math/math.inl"
  "D:/Library/Source/voltengine/volt/include/volt/math/quat.inl"
  "D:/Library/Source/voltengine/volt/include/volt/math/vec2.inl"
  "D:/Library/Source/voltengine/volt/include/volt/math/vec3.inl"
  "D:/Library/Source/voltengine/volt/include/volt/math/vec4.inl"
  "D:/Library/Source/voltengine/volt/include/volt/modules.inl"
  "D:/Library/Source/voltengine/volt/include/volt/util/string.inl"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:68 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/src/*.cpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/src/volt/assets/asset.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/assets/assets.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/config.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/core/assets/text.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/component_storage.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/ecs.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/entity.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/system.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/world.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/error.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/log.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/modules.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/os/monitor.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/os/window.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/paths.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/pch.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/util/file.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/util/string.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/util/version.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/video.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/adapter.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/buffer.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/device.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/instance.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/queue.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/shader.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/surface.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/texture.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/texture_view.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/vk12.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_HEADERS at CMakeLists.txt:53 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/src/*.hpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/src/volt/pch.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/adapter.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/d3d12.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/device.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/instance.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/pipeline.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/shader.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/d3d12/window.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/adapter.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/buffer.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/device.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/instance.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/queue.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/shader.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/surface.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/texture.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/texture_view.hpp"
  "D:/Library/Source/voltengine/volt/src/volt/video/vk12/vk12.hpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:68 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/src/*.inl")
set(OLD_GLOB
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/build/CMakeFiles/cmake.verify_globs")
endif()
