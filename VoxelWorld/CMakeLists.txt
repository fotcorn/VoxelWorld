cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

set(GLSLShaderPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/glsl)
set(MeshPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/meshes)
set(TexturePath ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures)
set(UIPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui)

add_executable(VoxelWorld
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/RenderLoop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/ShaderProgram.cpp
	# ${CMAKE_CURRENT_SOURCE_DIR}/source/Mesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Texture.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/RenderChunk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/RenderChunkGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/WorldGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/WorldRenderer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/World.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/2d/Rect.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/2d/Image.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/ui/DOM.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/ui/UI.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/ui/Layout.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/debug_ui/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/debug_ui/imgui_impl_opengl3.cpp
)

target_link_libraries(VoxelWorld ${CONAN_LIBS} gumbo yogacore)

target_compile_definitions(VoxelWorld PRIVATE
	# Path for OpenGL shaders
	GLSL_SHADER_PATH="${GLSLShaderPath}"
	# Path for meshes
	MESH_PATH="${MeshPath}"
	# Path for textures
	TEXTURE_PATH="${TexturePath}"
  	# Path for textures
	UI_PATH="${UIPath}"
	# Enable GLM experimental features
	GLM_ENABLE_EXPERIMENTAL=1
	# Workaround for broken detection of clang c++11 stdlib
	# https://github.com/g-truc/glm/issues/620
	GLM_FORCE_CXX11=1
)

target_compile_features(VoxelWorld PRIVATE cxx_std_17)

target_include_directories(VoxelWorld PRIVATE include)
target_include_directories(VoxelWorld PRIVATE external)

add_custom_target(copy-resources ALL
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/VoxelWorld/resources)

if(MSVC)
  target_compile_options(VoxelWorld PRIVATE /W4)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER MATCHES "clang")
  target_compile_options(VoxelWorld PRIVATE -Wall -Wextra -pedantic)
endif()


#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=undefined")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address")

#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=leak")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=leak")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
