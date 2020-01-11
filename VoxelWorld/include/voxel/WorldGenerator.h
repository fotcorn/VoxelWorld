#ifndef WORLD_GENERATOR_H
#define WORLD_GENERATOR_H

#include <memory>
#include <unordered_map>

#include <glm/gtx/hash.hpp>
#include <glm/vec3.hpp>

#include "Chunk.h"
#include "PerlinNoise.h"

const int WATER_HEIGHT = CHUNK_HEIGHT / 5;

const int BLOCK_AIR = 0;

class WorldGenerator {
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> chunkCache;
    siv::PerlinNoise noise;

public:
    WorldGenerator();
    std::shared_ptr<Chunk> getChunk(const glm::ivec3& position);
};

#endif // !WORLD_GENERATOR_H
