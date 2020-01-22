#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <optional>
#include <unordered_set>

#include <glm/vec3.hpp>

#include "TextureAtlas.h"
#include "WorldGenerator.h"

enum class Side {
    TOP,
    BOTTOM,
    FRONT,
    BACK,
    LEFT,
    RIGHT,
};

class World {
public:
    void addBlock(TextureAtlas blockType);
    void removeBlock();

    void cameraChanged(glm::vec3 cameraPosition, glm::vec3 cameraDirection, int cameraChunkDistance);
    void simulationTick();

    std::shared_ptr<Chunk> getChunk(const glm::ivec3& position);

    std::optional<glm::ivec3> selectedChunkPosition;
    std::optional<glm::ivec3> selectedBlockPosition;
    std::optional<Side> selectedBlockSide;

private:
    WorldGenerator worldGenerator;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> world;
    std::unordered_set<glm::ivec3> simulationChunks;

    std::vector<glm::ivec3> waterBlockUpdates = {
        glm::ivec3(1, 0, 0),
        glm::ivec3(-1, 0, 0),
        glm::ivec3(0, 0, 1),
        glm::ivec3(0, 0, -1),
    };

    std::optional<std::tuple<glm::ivec3, glm::ivec3>> canCreateBlock(std::shared_ptr<Chunk> chunk,
                                                                     glm::ivec3 chunkPosition, glm::ivec3 originBlock,
                                                                     glm::ivec3 blockOffset);
};

#endif /* WORLD_H */
