#!/usr/bin/env bash
#mkdir build-emscripten
#cd build-emscripten
#conan install .. --build=missing -pr=../emscripten.profile
source activate.sh
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j16
mv bin/VoxelWorld VoxelWorld.bc
emcc VoxelWorld.bc -o VoxelWorld.html -s USE_GLFW=3 -s USE_WEBGL2=1 --preload-file VoxelWorld/resources -s DISABLE_EXCEPTION_CATCHING=0 -s EXCEPTION_DEBUG=1 -s ALLOW_MEMORY_GROWTH=1
