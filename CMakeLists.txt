cmake_minimum_required(VERSION 3.20)
project(Volt LANGUAGES CXX)

include("$ENV{VOLT_PATH}/cmake/Volt.cmake")

# Engine

include("$ENV{VOLT_PATH}/cmake/CreateSharedLibrary.cmake")

target_compile_definitions(Volt PRIVATE VOLT_BUILD)

target_precompile_headers(Volt PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/volt/pch.hpp"
)

target_link_libraries(Volt JSON::JSON)

# App (Special Target)

file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS
	"${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/app/*.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/app/*.inl"
)

add_executable(APP ${TARGET_SOURCES})

target_include_directories(APP PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/app/"
)

set_target_properties(APP PROPERTIES
    CXX_STANDARD 20
)

target_precompile_headers(APP PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/app/pch.hpp"
)

target_link_libraries(APP Volt::Volt)
