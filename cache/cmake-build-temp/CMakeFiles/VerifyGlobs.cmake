# CMAKE generated file: DO NOT EDIT!
# Generated by CMake Version 3.20
cmake_policy(SET CMP0009 NEW)

# TARGET_SOURCES at CMakeLists.txt:117 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/app/*.cpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/app/app.cpp"
  "D:/Library/Source/voltengine/volt/app/pch.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:117 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/app/*.hpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/app/pch.hpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:117 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/app/*.inl")
set(OLD_GLOB
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_HEADERS at CMakeLists.txt:49 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/include/*.hpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/include/volt/assets/asset.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/assets/asset_ref.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/assets/assets.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/config.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/component_storage.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/ecs.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/entity.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/system.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/world.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/error.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/log.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/macros.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/modules.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/paths.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/file.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/static_block.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/string.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/util/util.hpp"
  "D:/Library/Source/voltengine/volt/include/volt/volt.hpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:64 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/include/*.inl")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/include/volt/assets/asset_ref.inl"
  "D:/Library/Source/voltengine/volt/include/volt/assets/assets.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/component_storage.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/ecs.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/entity.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/system.inl"
  "D:/Library/Source/voltengine/volt/include/volt/ecs/world.inl"
  "D:/Library/Source/voltengine/volt/include/volt/modules.inl"
  "D:/Library/Source/voltengine/volt/include/volt/util/string.inl"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:64 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/src/*.cpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/src/volt/assets/assets.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/config.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/component_storage.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/ecs.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/entity.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/system.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/ecs/world.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/error.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/log.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/modules.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/paths.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/pch.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/util/file.cpp"
  "D:/Library/Source/voltengine/volt/src/volt/util/string.cpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_HEADERS at CMakeLists.txt:49 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/src/*.hpp")
set(OLD_GLOB
  "D:/Library/Source/voltengine/volt/src/volt/pch.hpp"
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()

# TARGET_SOURCES at CMakeLists.txt:64 (file)
file(GLOB_RECURSE NEW_GLOB LIST_DIRECTORIES false "D:/Library/Source/voltengine/volt/src/*.inl")
set(OLD_GLOB
  )
if(NOT "${NEW_GLOB}" STREQUAL "${OLD_GLOB}")
  message("-- GLOB mismatch!")
  file(TOUCH_NOCREATE "D:/Library/Source/voltengine/volt/cache/cmake-build-temp/CMakeFiles/cmake.verify_globs")
endif()