#ifndef WORLD_H
#define WORLD_H

#include <memory>

#include <glm/vec3.hpp>

#include "WorldGenerator.h"

class World {
public:
    void addBlock();
    void removeBlock();

    void cameraChanged(glm::vec3 cameraPosition, glm::vec3 cameraDirection, int cameraChunkDistance);
    void simulationTick();

    std::shared_ptr<Chunk> getChunk(const glm::ivec3& position);

private:
    WorldGenerator worldGenerator;
    std::unordered_map<glm::ivec3, std::shared_ptr<Chunk>> world;
    std::shared_ptr<glm::ivec3> selectedChunkPosition;
    std::shared_ptr<glm::ivec3> selectedBlockPosition;
};

#endif /* WORLD_H */
