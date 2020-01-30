#!/usr/bin/env bash
set -euxo pipefail
#mkdir build-emscripten
#cd build-emscripten
#conan install .. --build=missing -pr=../emscripten.profile
source activate.sh
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j16
mv bin/VoxelWorld VoxelWorld.bc
emcc VoxelWorld.bc -o VoxelWorld.html -s USE_GLFW=3 -s USE_WEBGL2=1 --preload-file VoxelWorld/resources --shell-file ../emscripten_shell.html
