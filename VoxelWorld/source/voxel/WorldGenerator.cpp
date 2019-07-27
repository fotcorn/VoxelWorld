#include "voxel/WorldGenerator.h"
#include "TextureAtlas.h"

const int NOISE_SCALE = 10;
const int WATER_HEIGHT = 6;

WorldGenerator::WorldGenerator() : noise(1) {
}

Chunk WorldGenerator::getChunk(int x, int y, int z) {
    auto cacheEntry = chunkCache.find(glm::ivec3(x, y, z));
    if (cacheEntry != chunkCache.end()) {
        return cacheEntry->second;
    }

    Chunk chunk;

    // basic world generation
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            const double value = noise.noise0_1(double(x) / double(NOISE_SCALE), double(z) / double(NOISE_SCALE));
            const int height = value * CHUNK_SIZE;
            for (int y = 0; y < height; y++) {
                if (y > CHUNK_SIZE * 0.7) {
                    chunk(x, y, z) = TextureAtlas::SNOW;
                } else if (y > CHUNK_SIZE * 0.5) {
                    chunk(x, y, z) = TextureAtlas::STONE_04;
                } else {
                    chunk(x, y, z) = TextureAtlas::GROUND_EARTH;
                }
            }
            for (int y = height; y < CHUNK_SIZE; y++) {
                chunk(x, y, z) = BLOCK_AIR;
            }
        }
    }

    // water
    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int z = 0; z < CHUNK_SIZE; z++) {
            for (int y = 0; y <= WATER_HEIGHT; y++) {
                if (chunk(x, y, z) == BLOCK_AIR) {
                    chunk(x, y, z) = TextureAtlas::WATER;
                }
            }
            if (chunk(x, WATER_HEIGHT, z) == TextureAtlas::GROUND_EARTH && chunk(x, WATER_HEIGHT + 1, z) == BLOCK_AIR) {
                chunk(x, WATER_HEIGHT, z) = TextureAtlas::WATER;
            }
        }
    }

    return chunk;
}
