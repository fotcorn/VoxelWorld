from conans import ConanFile, CMake

class VoxelWorld(ConanFile):
  settings = 'os', 'compiler', 'build_type', 'arch'
  requires = [
    'glew/2.2.0',
    'glfw/3.3.8',
    'glm/cci.20220420',
    'stb/cci.20210910',
    'imgui/cci.20220621+1.88.docking',
    'fmt/9.0.0',
    'boost/1.79.0',
  ]
  generators = 'cmake',

  default_options = {
    'boost:header_only': True,
    'fmt:header_only': True,
  }

  def build(self):
    cmake = CMake(self)
    cmake.configure()
    cmake.build()
