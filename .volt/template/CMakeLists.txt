cmake_minimum_required(VERSION 3.20)
# project(Example LANGUAGES CXX)

# Make dependencies available and set output directories
# This is technically only required in the top-level CMakeLists.txt
# Can be ommited if package is not meant to be built on its own
include("$ENV{VOLT_PATH}/cmake/Volt.cmake")

# Define new shared library target and set it up as a module
# 
# volt_add_module(${PROJECT_NAME))

# This macro will be defined only when building
# the shared library, use it to export symbols
# 
# target_compile_definitions(${PROJECT_NAME} PRIVATE
# 	EXAMPLE_EXPORTS
# )

# Add targets provided by dependencies
# Package should explicitly define those dependencies
# and not rely on other packages providing them
# 
# target_link_libraries(${PROJECT_NAME}
# 	Volt::Volt
# )
