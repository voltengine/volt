cmake_minimum_required(VERSION 3.20)
project(Volt LANGUAGES CXX)

include("$ENV{VOLT_PATH}/cmake/Volt.cmake")

# Extracts "name" from "scope/name"
function(volt_get_name_from_id id output_name)
	string(FIND ${id} "/" SLASH_INDEX)
	math(EXPR SLASH_INDEX "${SLASH_INDEX} + 1")
	string(SUBSTRING ${id} ${SLASH_INDEX} -1 NAME)
	set(${output_name} ${NAME} PARENT_SCOPE)
endfunction()

# Set build options
set(VOLT_DEVELOPMENT true CACHE BOOL "Build app with development features (i.e. the editor and additional error checks).")
set(VOLT_MAX_COMPONENTS "128" CACHE STRING "Maximum number of registered components.")
set(VOLT_GPU_DEBUG false CACHE BOOL "Build app with graphics API debugging enabled.")

# Read root package name into $CACHE{VOLT_DEVELOPMENT_MODULE}
if(VOLT_DEVELOPMENT)
	file(READ "${CMAKE_SOURCE_DIR}/package.json" VOLT_PACKAGE_JSON)
	string(JSON VOLT_PACKAGE_ID GET ${VOLT_PACKAGE_JSON} "id")
	volt_get_name_from_id(${VOLT_PACKAGE_ID} VOLT_PACKAGE_NAME)
	set(VOLT_DEVELOPMENT_MODULE ${VOLT_PACKAGE_NAME} CACHE INTERNAL "")
	unset(VOLT_PACKAGE_JSON)
	unset(VOLT_PACKAGE_ID)
	unset(VOLT_PACKAGE_NAME)

	message("[Volt] Development is enabled.")
else()
	message("[Volt] Development is disabled.")
endif()

# Read version into $CACHE{VOLT_VERSION}
file(READ "${CMAKE_SOURCE_DIR}/package.json" VOLT_PACKAGE_JSON)
string(JSON VOLT_PACKAGE_VERSION GET ${VOLT_PACKAGE_JSON} "version")
set(VOLT_VERSION ${VOLT_PACKAGE_VERSION} CACHE INTERNAL "")
unset(VOLT_PACKAGE_JSON)
unset(VOLT_PACKAGE_VERSION)

# Creates shared library target named ${PROJECT_NAME}
# (aliased as ${PROJECT_NAME}::${PROJECT_NAME}),
# adds C++ sources from ./src/ and ./include/ and
# defines C++ 20 as the required standard
# Additionally generates module_main from annotations
# Can be called only just once for each package
set(VOLT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR} CACHE INTERNAL "")
function(volt_add_module target)
	# Detect information about the package
	file(READ "${CMAKE_CURRENT_SOURCE_DIR}/package.json" VOLT_PACKAGE_JSON)
	string(JSON VOLT_PACKAGE_ID GET ${VOLT_PACKAGE_JSON} "id")
	volt_get_name_from_id(${VOLT_PACKAGE_ID} VOLT_PACKAGE_NAME)
	message("[Volt] Adding module: ${target} (${VOLT_PACKAGE_NAME})")

	# Discover headers
	file(GLOB_RECURSE TARGET_HEADERS CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
	)

	# Transform list of headers into a string
	# of space-separated command line arguments
	list(TRANSFORM TARGET_HEADERS PREPEND "\"" OUTPUT_VARIABLE QUOTED_TARGET_HEADERS)
	list(TRANSFORM QUOTED_TARGET_HEADERS APPEND "\"")
	list(JOIN QUOTED_TARGET_HEADERS " " SPACE_SEPARATED_HEADERS)

	string(REPLACE "-" "_" GENERATED_SOURCE_NAME ${VOLT_PACKAGE_NAME})
	set(GENERATED_SOURCE "${CMAKE_BINARY_DIR}/generated/${GENERATED_SOURCE_NAME}.cpp")

	# Discover sources and append them with headers plus the generated source
	file(GLOB_RECURSE TARGET_SOURCES CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/include/*.inl"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.inl"
	)
	list(APPEND TARGET_SOURCES ${TARGET_HEADERS} ${GENERATED_SOURCE})

	# Add the target
	add_library(${target} SHARED ${TARGET_SOURCES})
	add_library(${target}::${target} ALIAS ${target})

	target_include_directories(${target} PRIVATE
		"${CMAKE_CURRENT_SOURCE_DIR}/src/"
	)
	target_include_directories(${target} PUBLIC
		"${CMAKE_CURRENT_SOURCE_DIR}/include/"
	)

	set_target_properties(${target} PROPERTIES
		CXX_STANDARD 20
		OUTPUT_NAME ${VOLT_PACKAGE_NAME}
	)

	# Add precompiler definitions and set build type
	target_compile_definitions(${target} PUBLIC VOLT_MAX_COMPONENTS=${VOLT_MAX_COMPONENTS})
	target_compile_definitions(${target} PUBLIC VOLT_VERSION="$CACHE{VOLT_VERSION}")
	if(VOLT_DEVELOPMENT)
		target_compile_definitions(${target} PUBLIC VOLT_DEVELOPMENT)
		target_compile_definitions(${target} PUBLIC VOLT_DEVELOPMENT_PATH="${CMAKE_SOURCE_DIR}")
		target_compile_definitions(${target} PUBLIC VOLT_DEVELOPMENT_MODULE="$CACHE{VOLT_DEVELOPMENT_MODULE}")
	endif()
	if (VOLT_GPU_DEBUG)
		target_compile_definitions(${target} PUBLIC VOLT_GPU_DEBUG)
	endif()

	# Add module_main source file generation step
	add_custom_command(
		OUTPUT ${GENERATED_SOURCE}
		COMMAND "python" "$CACHE{VOLT_LIST_DIR}/generate_module_main.py" ${VOLT_DEVELOPMENT} "${CMAKE_CURRENT_SOURCE_DIR}" "${GENERATED_SOURCE}" ${SPACE_SEPARATED_HEADERS}
		DEPENDS ${TARGET_HEADERS}
	)
endfunction()

# Add Engine module

volt_add_module(${PROJECT_NAME})

target_compile_definitions(${PROJECT_NAME} PRIVATE
	VOLT_EXPORTS
)

target_link_libraries(${PROJECT_NAME}
	Date::Date
	GLFW::GLFW
	JSON::JSON
	Termcolor::Termcolor
)

# Include Vulkan on non-Apple operating systems
if(NOT APPLE)
	target_link_libraries(${PROJECT_NAME}
		Glad::Glad
		SPIRV-Reflect::SPIRV-Reflect
		VulkanMemoryAllocator::VulkanMemoryAllocator
	)
endif()

# Include Direct3D 12 on Windows
if(WIN32)
	target_link_libraries(${PROJECT_NAME}
		D3D12MemoryAllocator::D3D12MemoryAllocator
		"d3d12.lib" "dxgi.lib" "d3dcompiler.lib"
	)
endif()

target_precompile_headers(${PROJECT_NAME} PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/src/volt/pch.hpp"
)

# Add app custom target

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

target_link_libraries(APP
	Glad::Glad
	GLFW::GLFW
	Volt::Volt
)
