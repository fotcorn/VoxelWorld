#!/usr/bin/env bash
set -euxo pipefail
cp VoxelWorld.{data,html,js,wasm} ../../emscriptenVoxelWorld
pushd ../../emscriptenVoxelWorld
mv VoxelWorld.html index.html
git add .
git commit -a -m "update emscripten"
