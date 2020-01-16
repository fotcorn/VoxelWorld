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
                                    selectedBlockSide = std::nullopt;
                                    minimalDistance = distance.value();
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // check which side of the block is selected
    if (selectedChunkPosition.has_value() && selectedBlockPosition.has_value()) {
        auto chunkPosition = glm::vec3(selectedChunkPosition.value());
        float offset = 1.0f / float(CHUNK_SIZE);

        auto bottomLeftBack = chunkPosition + (glm::vec3(selectedBlockPosition.value()) / float(CHUNK_SIZE));
        auto topRightFront = chunkPosition + (glm::vec3(selectedBlockPosition.value()) / float(CHUNK_SIZE)) +
                             glm::vec3(offset, offset, offset);

        using ListEntry = std::tuple<Side, std::optional<float>>;

        auto list = std::vector<ListEntry>{
            ListEntry(Side::LEFT, ray.AABBintersect(bottomLeftBack, bottomLeftBack + glm::vec3(0.0f, offset, offset))),
            ListEntry(Side::BOTTOM,
                      ray.AABBintersect(bottomLeftBack, bottomLeftBack + glm::vec3(offset, 0.0f, offset))),
            ListEntry(Side::BACK, ray.AABBintersect(bottomLeftBack, bottomLeftBack + glm::vec3(offset, offset, 0.0f))),

            ListEntry(Side::RIGHT, ray.AABBintersect(topRightFront, topRightFront + glm::vec3(0.0f, -offset, -offset))),
            ListEntry(Side::TOP, ray.AABBintersect(topRightFront, topRightFront + glm::vec3(-offset, 0.0f, -offset))),
            ListEntry(Side::FRONT,
                      ray.AABBintersect(topRightFront, topRightFront + glm::vec3(-offset, -offset, 0.0f)))};

        minimalDistance = std::numeric_limits<float>::max();
        for (auto& item : list) {
            auto distance = std::get<1>(item);
            if (distance.has_value() && distance.value() < minimalDistance) {
                minimalDistance = distance.value();
                selectedBlockSide = std::get<0>(item);
            }
        }
    }
}

void World::addBlock() {
    if (selectedChunkPosition) {
        auto chunk = world[*selectedChunkPosition];

        glm::ivec3 position = selectedBlockPosition.value();
        switch (selectedBlockSide.value()) {
        case Side::RIGHT:
            position += glm::ivec3(1, 0, 0);
            break;
        case Side::LEFT:
            position += glm::ivec3(-1, 0, 0);
            break;
        case Side::TOP:
            position += glm::ivec3(0, 1, 0);
            break;
        case Side::BOTTOM:
            position += glm::ivec3(0, -1, 0);
            break;
        case Side::FRONT:
            position += glm::ivec3(0, 0, 1);
            break;
        case Side::BACK:
            position += glm::ivec3(0, 0, -1);
            break;
        }

        if ((*chunk)(position.x, position.y, position.z) == BLOCK_AIR) {
            (*chunk)(position.x, position.y, position.z) = TextureAtlas::LAVA;
            chunk->changed = true;
        }
        simulationChunks.insert(selectedChunkPosition.value());
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
    for (auto& chunkPosition : simulationChunks) {
        auto chunk = world[chunkPosition];
        std::vector<glm::ivec3> newBlocks;
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if ((*chunk)(x, y, z) == TextureAtlas::LAVA) {
                        if ((*chunk)(x + 1, y, z) == BLOCK_AIR) {
                            newBlocks.push_back(glm::ivec3(x + 1, y, z));
                        }
                        if ((*chunk)(x - 1, y, z) == BLOCK_AIR) {
                            newBlocks.push_back(glm::ivec3(x - 1, y, z));
                        }
                        if ((*chunk)(x, y - 1, z) == BLOCK_AIR) {
                            newBlocks.push_back(glm::ivec3(x, y + 1, z));
                        }
                        if ((*chunk)(x, y, z + 1) == BLOCK_AIR) {
                            newBlocks.push_back(glm::ivec3(x, y - 1, z));
                        }
                        if ((*chunk)(x, y, z - 1) == BLOCK_AIR) {
                            newBlocks.push_back(glm::ivec3(x, y, z - 1));
                        }
                    }
                }
            }
        }

        for (auto& block : newBlocks) {
            (*chunk)(block.x, block.y, block.z) = TextureAtlas::LAVA;
        }

        chunk->changed = true;
    }
}
