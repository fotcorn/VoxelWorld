from conans import ConanFile, CMake

class VoxelWorld(ConanFile):
  settings = 'os', 'compiler', 'build_type', 'arch'
  requires = [
    'glew/2.1.0@bincrafters/stable',
    'glfw/3.2.1@bincrafters/stable',
    'glm/0.9.9.1@g-truc/stable',
    'stb/20180214@conan/stable',
    'imgui/1.62@bincrafters/stable',
    'fmt/5.2.1@bincrafters/stable',
    'boost/1.70.0@conan/stable',
  ]
  generators = 'cmake',

  default_options = {
    'boost:header_only': True,
  }

  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()
