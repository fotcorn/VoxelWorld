cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(VoxelRendering
	LANGUAGES CXX
	VERSION 0.1.0
)

set(CONAN_SYSTEM_INCLUDES ON)

# use headers and link libraries downloaded by conan
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(external/yoga)
add_subdirectory(external/gumbo)
add_subdirectory(VoxelWorld)
