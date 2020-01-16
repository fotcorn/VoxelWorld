#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <optional>
#include <unordered_set>

#include <glm/vec3.hpp>

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
    void addBlock();
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
};

#endif /* WORLD_H */
