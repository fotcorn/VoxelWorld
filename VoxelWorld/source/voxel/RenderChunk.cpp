#include "voxel/RenderChunk.h"
#include "voxel/WorldGenerator.h"

static bool needsRender(const Chunk& chunk, const int x, const int y, const int z) {
    if (x < 0 || y < 0 || z < 0) {
        return false;
    }
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
        return true;
    }
    return chunk(x, y, z) == BLOCK_AIR;
}

static bool isVisible(const Chunk& chunk, const int x, const int y, const int z) {
    return needsRender(chunk, x - 1, y, z) || needsRender(chunk, x + 1, y, z) || needsRender(chunk, x, y - 1, z) ||
           needsRender(chunk, x, y + 1, z) || needsRender(chunk, x, y, z - 1) || needsRender(chunk, x, y, z + 1);
}

RenderChunk RenderChunk::fromChunk(const WorldGenerator& worldGenerator, const Chunk& chunk) {
    RenderChunk renderChunk;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk(x, y, z) == BLOCK_AIR) {
                    continue;
                }
                if (!isVisible(chunk, x, y, z)) {
                    continue;
                }

                /*
                std::vector<Vertex> vertices;
                std::vector<unsigned int> indices;
                struct Vertex {
                    glm::vec3 position = glm::vec3();
                    glm::vec2 texturePosition = glm::vec3();
                };
                */

                // glm::mat4 cubeModel = glm::mat4(1.0f);
                // glm::vec3 cubePosition = glm::vec3(x, y, z);
                // cubeModel = glm::translate(cubeModel, cubePosition);
                // blocks.push_back({cubeModel, glm::ivec3(x, y, z), (TextureAtlas)(*world)(x, y, z)});
            }
        }
    }

    renderChunk.setupRenderData();
    return renderChunk;
}
