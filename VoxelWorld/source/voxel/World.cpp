#include "voxel/World.h"
#include "TextureAtlas.h"
#include "voxel/Ray.h"

#include <iostream>

std::shared_ptr<Chunk> World::getChunk(const glm::ivec3& position) {
    std::shared_ptr<Chunk> chunk;

    auto existingChunk = world.find(position);
    if (existingChunk != world.end()) {
        chunk = existingChunk->second;
    } else {
        chunk = worldGenerator.generateChunk(position);
    }
    world[position] = chunk;

    if (selectedChunkPosition && selectedChunkPosition.value() == position) {
        chunk->tempChanged = true;
    }
    return chunk;
}

void World::cameraChanged(glm::vec3 cameraPosition, glm::vec3 cameraDirection, int cameraChunkDistance) {
    const int currentX = int(cameraPosition.x);
    const int currentZ = int(cameraPosition.z);
    auto ray = Ray(cameraPosition, cameraDirection);

    selectedChunkPosition.reset();
    selectedBlockPosition.reset();
    float minimalDistance = std::numeric_limits<float>::max();

    for (int x = currentX - cameraChunkDistance; x < currentX + cameraChunkDistance; x++) {
        for (int z = currentZ - cameraChunkDistance; z < currentZ + cameraChunkDistance; z++) {
            const auto position = glm::ivec3(x, 1, z);
            const auto floatPosition = glm::vec3(position);

            auto topRight = floatPosition + glm::vec3(1.0f, 4.0f, 1.0f);

            if (ray.AABBintersect(floatPosition, topRight).has_value()) {
                auto chunk = this->getChunk(position);
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int y = 0; y < CHUNK_HEIGHT; y++) {
                        for (int z = 0; z < CHUNK_SIZE; z++) {
                            if ((*chunk)(x, y, z) == BLOCK_AIR) {
                                continue;
                            }

                            auto bottomLeft = floatPosition + glm::vec3(float(x) / CHUNK_SIZE, float(y) / CHUNK_SIZE,
                                                                        float(z) / CHUNK_SIZE);
                            topRight =
                                floatPosition + glm::vec3(float(x + 1.0f) / CHUNK_SIZE, float(y + 1.0f) / CHUNK_SIZE,
                                                          float(z + 1.0f) / CHUNK_SIZE);

                            auto distance = ray.AABBintersect(bottomLeft, topRight);
                            if (distance.has_value()) {
                                if (distance < minimalDistance) {
                                    selectedChunkPosition = position;
                                    selectedBlockPosition = glm::ivec3(x, y, z);
                                    selectedBlockSide = Side::TOP;
                                    minimalDistance = distance.value();
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void World::addBlock() {
    if (selectedChunkPosition) {
        auto chunk = world[*selectedChunkPosition];
        if ((*chunk)(selectedBlockPosition->x, selectedBlockPosition->y + 1, selectedBlockPosition->z) == BLOCK_AIR) {
            (*chunk)(selectedBlockPosition->x, selectedBlockPosition->y + 1, selectedBlockPosition->z) =
                TextureAtlas::GROUND_MUD;
            chunk->changed = true;
        }
    }
}

void World::removeBlock() {
    if (selectedChunkPosition) {
        auto chunk = world[*selectedChunkPosition];
        (*chunk)(selectedBlockPosition->x, selectedBlockPosition->y, selectedBlockPosition->z) = BLOCK_AIR;
        chunk->changed = true;
    }
}

void World::simulationTick() {
    std::cout << "simulation tick" << std::endl;
}
