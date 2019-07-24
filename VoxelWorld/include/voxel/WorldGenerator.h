#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>

#include "Tensor3.h"

const int CHUNK_SIZE = 16;

using Chunk = Tensor3<char, CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE>;

class WorldGenerator {
    std::unordered_map<glm::ivec3, Chunk> chunkCache;

public:
    Chunk getChunk(int x, int y, int z);
};

#endif // !WORLD_GENERATOR_H
