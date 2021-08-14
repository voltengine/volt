cmake_minimum_required(VERSION 3.20)
project(Volt LANGUAGES CXX)

include("$ENV{VOLT_PATH}/cmake/Volt.cmake")

# Engine

include("$ENV{VOLT_PATH}/cmake/CreateSharedLibrary.cmake")

target_compile_definitions(${PROJECT_NAME} PRIVATE
	VOLT_IS_CURRENT_BUILD_TARGET
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "volt"
)

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
    OUTPUT_NAME "${VOLT_ROOT_NAME}-app"
)

target_link_libraries(APP
	${PROJECT_NAME}::${PROJECT_NAME}
)
