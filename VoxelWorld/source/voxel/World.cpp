#include "voxel/World.h"
#include "TextureAtlas.h"
#include "voxel/Ray.h"

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

void World::addBlock(TextureAtlas blockType) {
    if (selectedChunkPosition) {
        glm::ivec3 blockOffset;
        switch (selectedBlockSide.value()) {
        case Side::RIGHT:
            blockOffset = glm::ivec3(1, 0, 0);
            break;
        case Side::LEFT:
            blockOffset = glm::ivec3(-1, 0, 0);
            break;
        case Side::TOP:
            blockOffset = glm::ivec3(0, 1, 0);
            break;
        case Side::BOTTOM:
            blockOffset = glm::ivec3(0, -1, 0);
            break;
        case Side::FRONT:
            blockOffset = glm::ivec3(0, 0, 1);
            break;
        case Side::BACK:
            blockOffset = glm::ivec3(0, 0, -1);
            break;
        }

        const auto [newChunkPosition, newBlockPosition] =
            this->getRelativeChunkPosition(selectedChunkPosition.value(), selectedBlockPosition.value(), blockOffset);

        auto chunk = this->getChunk(newChunkPosition);
        if ((*chunk)(newBlockPosition.x, newBlockPosition.y, newBlockPosition.z) == BLOCK_AIR) {
            (*chunk)(newBlockPosition.x, newBlockPosition.y, newBlockPosition.z) = blockType;
            chunk->changed = true;
        }
        simulationChunks.insert(newChunkPosition);
    }
}

void World::removeBlock() {
    if (selectedChunkPosition) {
        auto chunk = world[*selectedChunkPosition];
        (*chunk)(selectedBlockPosition->x, selectedBlockPosition->y, selectedBlockPosition->z) = BLOCK_AIR;
        chunk->changed = true;
    }
}

std::tuple<glm::ivec3, glm::ivec3> World::getRelativeChunkPosition(glm::ivec3 chunkPosition, glm::ivec3 originBlock,
                                                                   glm::ivec3 blockOffset) {
    glm::ivec3 newChunkPosition = chunkPosition;
    std::shared_ptr<Chunk> newChunk;
    glm::ivec3 newBlockPosition = originBlock + blockOffset;

    // handle possibilty that new block is outside of current chunk
    if (newBlockPosition.x < 0) {
        newChunkPosition = chunkPosition + glm::ivec3(-1, 0, 0);
        newChunk = this->getChunk(newChunkPosition);
        newBlockPosition = glm::ivec3(CHUNK_SIZE - 1, newBlockPosition.y, newBlockPosition.z);
    }

    if (newBlockPosition.x == CHUNK_SIZE) {
        newChunkPosition = chunkPosition + glm::ivec3(1, 0, 0);
        newChunk = this->getChunk(newChunkPosition);
        newBlockPosition = glm::ivec3(0, newBlockPosition.y, newBlockPosition.z);
    }

    if (newBlockPosition.z < 0) {
        newChunkPosition = chunkPosition + glm::ivec3(0, 0, -1);
        newChunk = this->getChunk(newChunkPosition);
        newBlockPosition = glm::ivec3(newBlockPosition.x, newBlockPosition.y, CHUNK_SIZE - 1);
    }

    if (newBlockPosition.z == CHUNK_SIZE) {
        newChunkPosition = chunkPosition + glm::ivec3(0, 0, 1);
        newChunk = this->getChunk(newChunkPosition);
        newBlockPosition = glm::ivec3(newBlockPosition.x, newBlockPosition.y, 0);
    }

    if (!newChunk) {
        newChunk = this->getChunk(chunkPosition);
    }
    return std::make_tuple(newChunkPosition, newBlockPosition);
}

std::optional<std::tuple<glm::ivec3, glm::ivec3>> World::canCreateBlock(std::shared_ptr<Chunk> chunk,
                                                                        glm::ivec3 chunkPosition,
                                                                        glm::ivec3 originBlock,
                                                                        glm::ivec3 blockOffset) {
    const auto [newChunkPosition, newBlockPosition] =
        this->getRelativeChunkPosition(chunkPosition, originBlock, blockOffset);
    const auto newChunk = this->getChunk(newChunkPosition);

    // if new block is already occupied, do not overwrite block
    if ((*newChunk)(newBlockPosition) != BLOCK_AIR) {
        return std::nullopt;
    }

    // if we are at the lowest possible position, uncondinationally add the new block
    if (newBlockPosition.y == 0) {
        return std::make_tuple(newChunkPosition, newBlockPosition);
    }

    // if the block below the new block is not air and not water, add the block
    const auto blockBelowType = (*newChunk)(newBlockPosition + glm::ivec3(0, -1, 0));
    if (blockBelowType != BLOCK_AIR && blockBelowType != TextureAtlas::WATER) {
        return std::make_tuple(newChunkPosition, newBlockPosition);
    }

    // if the block below the new block is air, but the block below the origin block is not air/water, add block anyway
    // (think waterfall)
    const auto originBlockType = (*chunk)(originBlock + glm::ivec3(0, -1, 0));
    if (originBlockType != TextureAtlas::WATER && originBlockType != BLOCK_AIR) {
        return std::make_tuple(newChunkPosition, newBlockPosition);
    }

    return std::nullopt;
}

void World::simulationTick() {
    std::unordered_map<glm::ivec3, std::vector<glm::ivec3>> newBlocks;

    for (auto& chunkPosition : simulationChunks) {
        auto chunk = world[chunkPosition];
        for (int x = 0; x < CHUNK_SIZE; x++) {
            for (int y = 0; y < CHUNK_HEIGHT; y++) {
                for (int z = 0; z < CHUNK_SIZE; z++) {
                    if ((*chunk)(x, y, z) == TextureAtlas::WATER) {
                        // block below current block
                        glm::ivec3 blockPositionBelow = glm::ivec3(x, y - 1, z);
                        if (y != 0 && (*chunk)(blockPositionBelow) == BLOCK_AIR) {
                            if (newBlocks.find(chunkPosition) == newBlocks.end()) {
                                newBlocks[chunkPosition] = std::vector<glm::ivec3>{blockPositionBelow};
                            } else {
                                newBlocks[chunkPosition].push_back(blockPositionBelow);
                            }
                        }

                        // blocks around current block
                        for (auto& blockUpdate : waterBlockUpdates) {
                            const auto ret =
                                this->canCreateBlock(chunk, chunkPosition, glm::ivec3(x, y, z), blockUpdate);
                            if (ret.has_value()) {
                                auto [chunkPos, blockPos] = ret.value();
                                if (newBlocks.find(chunkPos) == newBlocks.end()) {
                                    newBlocks[chunkPos] = std::vector<glm::ivec3>{blockPos};
                                } else {
                                    newBlocks[chunkPos].push_back(blockPos);
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    simulationChunks.clear();

    for (const auto& [chunkPosition, blocks] : newBlocks) {
        const auto chunk = world[chunkPosition];
        for (const auto& block : blocks) {
            (*chunk)(block.x, block.y, block.z) = TextureAtlas::WATER;
        }
        chunk->changed = true;
        simulationChunks.insert(chunkPosition);
    }
}
