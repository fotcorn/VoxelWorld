#include "voxel/WorldGenerator.h"

Chunk WorldGenerator::getChunk(int x, int y, int z) {
    // TODO: check if in cache

    Chunk tensor;
    // TODO

    /*
    siv::PerlinNoise noise(1);

    auto world = std::make_shared<WorldTensor>();

    // basic world generation
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            const double value = noise.noise0_1(double(x) / double(NOISE_SCALE), double(z) / double(NOISE_SCALE));
            const int height = value * WORLD_Y;
            for (int y = 0; y < height; y++) {
                if (y > WORLD_Y * 0.7) {
                    (*world)(x, y, z) = TextureAtlas::SNOW;
                } else if (y > WORLD_Y * 0.5) {
                    (*world)(x, y, z) = TextureAtlas::STONE_04;
                } else {
                    (*world)(x, y, z) = TextureAtlas::GROUND_EARTH;
                }
            }
        }
    }

    // water
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            for (int y = 0; y <= WATER_HEIGHT; y++) {
                if ((*world)(x, y, z) == BLOCK_AIR) {
                    (*world)(x, y, z) = TextureAtlas::WATER;
                }
            }
            if ((*world)(x, WATER_HEIGHT, z) == TextureAtlas::GROUND_EARTH &&
                (*world)(x, WATER_HEIGHT + 1, z) == BLOCK_AIR) {
                (*world)(x, WATER_HEIGHT, z) = TextureAtlas::WATER;
            }
        }
    }

    */

    return tensor;
}
