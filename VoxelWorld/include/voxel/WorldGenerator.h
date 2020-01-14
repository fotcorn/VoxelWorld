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
    siv::PerlinNoise noise;

public:
    WorldGenerator();
    std::shared_ptr<Chunk> generateChunk(const glm::ivec3& position);
};

#endif // !WORLD_GENERATOR_H
