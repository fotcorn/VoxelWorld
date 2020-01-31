# VoxelWorld

Play it in your browser now: https://fotcorn.github.io/VoxelWorld/

## Screenshot

![Screentshot](https://raw.githubusercontent.com/fotcorn/VoxelWorld/master/screenshot.png)

## Features
* Procedurally generated endless voxel world
* Optimized vertex data generation from voxel world
* OpenGL 4 based renderer
* Simple water simulation
* Adding/removing blocks with raycasting from mouse position to 3D block position
* UI library based on HTML/CSS with flexbox layout support

## Dependencies

Install the following tools:

- conan
- cmake

## Setup and build

### Linux (gcc/clang)

```
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install .. --build=missing
cmake ..
cmake --build .
```

### Windows (Visual Studio)

```
mkdir build
cd build
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
conan install .. -s build_type=Debug -s compiler="Visual Studio" -s compiler.runtime=MDd --build=missing
cmake .. -G "Visual Studio 15 2017 Win64"
cmake --build .
```

## Sources

- Block assets: https://opengameart.org/content/free-low-poly-game-asset-3d-blocks
- Perlin noise library: https://github.com/Reputeless/PerlinNoise
- LearnOpenGL.com


