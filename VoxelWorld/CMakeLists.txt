# File: VoxelWorld/CMakeLists.txt
cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

# Dependencies
find_package(glfw REQUIRED)
find_package(glew REQUIRED)
find_package(stb REQUIRED)
find_package(fmt REQUIRED)
find_package(imgui REQUIRED)
find_package(Assimp REQUIRED)
# TODO Remove
find_package(OpenGL REQUIRED)



set(GLSLShaderPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/glsl)
set(SPIRVShaderSource ${CMAKE_CURRENT_SOURCE_DIR}/resources/shaders/spir-v)
set(SPIRVShaderPath ${CMAKE_CURRENT_BINARY_DIR})
set(MeshPath ${CMAKE_CURRENT_SOURCE_DIR}/resources/meshes)
set(TexturePath ${CMAKE_CURRENT_SOURCE_DIR}/resources/textures)


add_executable(VoxelWorld
	${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/RenderLoop.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Shader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/ShaderProgram.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Mesh.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/Texture.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/RenderChunk.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/RenderChunkGL.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/WorldGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/voxel/WorldRenderer.cpp

	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/imgui_impl_glfw.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/source/gui/imgui_impl_opengl3.cpp
)

# TODO Remove OpenGL & GLEW
target_link_libraries(VoxelWorld PRIVATE OpenGL::GL glew::glew glfw::glfw fmt::fmt stb::stb imgui::imgui Assimp::Assimp Voxel::Voxel Voxel::OpenGLRenderer)

target_compile_definitions(VoxelWorld PRIVATE
	# Path for OpenGL shaders
	GLSL_SHADER_PATH="${GLSLShaderPath}"
	# Path for Vulkan (Spir-V) shaders
	SPIRV_SHADER_PATH="${SPIRVShaderPath}"
	# Path for meshes
	MESH_PATH="${MeshPath}"
	# Path for textures
	TEXTURE_PATH="${TexturePath}"
)

target_compile_features(VoxelWorld PRIVATE cxx_std_11)

target_include_directories(VoxelWorld PRIVATE include)
target_include_directories(VoxelWorld PRIVATE external)

# Add header files to IDE
file(GLOB Headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)
target_sources(VoxelWorld PRIVATE ${Headers})


add_custom_target(copy-resources ALL
    COMMAND cmake -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/VoxelWorld/resources)

# Setup GLSL shaders, i.e. add them to the IDE
set(GLSLShaders
	${GLSLShaderPath}/cube.vert
	${GLSLShaderPath}/cube.frag
)
foreach(GLSLShader IN LISTS GLSLShaders)
	string(REGEX REPLACE ${GLSLShaderPath} "" GLSLShaderFile ${GLSLShader})
	get_filename_component(Path ${GLSLShaderFile} DIRECTORY)
	source_group(Shaders\\GLSL\\${Path} FILES ${GLSLShader})
endforeach(GLSLShader)
target_sources(VoxelWorld PRIVATE ${GLSLShaders})


# Setup Spir-V shaders, i.e. add them to the IDE and add a command to compile them
#set(SPIRVShaders
#	${SPIRVShaderSource}/basic/basic.vert
#	${SPIRVShaderSource}/basic/basic.frag
#)
#unset(CompiledShaders)
#foreach(SPIRVShader IN LISTS SPIRVShaders)
#	string(REGEX REPLACE ${SPIRVShaderSource} "" SPIRVShaderFile ${SPIRVShader})
#	get_filename_component(File ${SPIRVShaderFile} NAME)
#	get_filename_component(Path ${SPIRVShaderFile} DIRECTORY)
#	source_group(Shaders\\SPIR-V\\${Path} FILES ${SPIRVShader})
#
#	set(CompiledShader "${Path}/${File}.spv")
#	add_custom_command(
#		OUTPUT "${CompiledShader}"
#		COMMAND "${CMAKE_COMMAND}" -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${Path}"
#		COMMAND "${GLSLValidator}" -V "${SPIRVShader}" -o "${SPIRVShaderPath}/${CompiledShader}"
#		DEPENDS "${SPIRVShader}"
#	)
#	list(APPEND CompiledShaders ${CompiledShader})
#endforeach(SPIRVShader)
#add_custom_target(VoxelWorld-CompileSpirV
#	DEPENDS ${CompiledShaders}
#)
#add_dependencies(VoxelWorld VoxelWorld-CompileSpirV)
#target_sources(VoxelWorld PRIVATE ${SPIRVShaders})

if(MSVC)
  target_compile_options(VoxelWorld PRIVATE /W4)
endif()
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER MATCHES "clang")
  target_compile_options(VoxelWorld PRIVATE -Wall -Wextra -pedantic)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
