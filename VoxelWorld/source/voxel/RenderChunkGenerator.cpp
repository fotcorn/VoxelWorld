#include "voxel/RenderChunkGenerator.h"
#include "voxel/WorldGenerator.h"

RenderChunkGenerator::RenderChunkGenerator() {
    cubeMesh = std::make_shared<Mesh>(Mesh::loadFromFile("block.obj"));
}

static bool needsRender(const Chunk& chunk, const int x, const int y, const int z) {
    if (x < 0 || y < 0 || z < 0) {
        return false;
    }
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
        return true;
    }
    return chunk(x, y, z) == BLOCK_AIR;
}

RenderChunk RenderChunkGenerator::fromChunk(const WorldGenerator& worldGenerator, const Chunk& chunk) {
    std::vector<Vertex> vertices;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk(x, y, z) == BLOCK_AIR) {
                    continue;
                }

                auto meshVertices = cubeMesh->getVertices();
                for (auto index : cubeMesh->getIndices()) {
                    auto vertex = meshVertices[index];
                    vertices.push_back(Vertex(vertex.position + glm::vec3(x, y, z), vertex.texturePosition));
                }

                // front
                if (needsRender(chunk, x, y, z + 1)) {
                    // add 6 vertices
                    // add 6 texture coordinates
                    // renderChunk.vertices.push_back(v(0, x, y, z));
                    // renderChunk.vertices.push_back(v(1, x, y, z));
                    // renderChunk.vertices.push_back(v(2, x, y, z));

                    // renderChunk.vertices.push_back(v(2, x, y, z));
                    // renderChunk.vertices.push_back(v(3, x, y, z));
                    // renderChunk.vertices.push_back(v(0, x, y, z));
                }

                // back
                if (needsRender(chunk, x, y, z - 1)) {
                    // vertices.append(v(7, 6, 5, x, y, z));
                    // vertices.append(v(5, 4, 7, x, y, z));
                }
                // right
                if (needsRender(chunk, x + 1, y, z)) {
                    // vertices.append(v(1, 5, 6, x, y, z));
                    // vertices.append(v(6, 2, 1, x, y, z));
                }
                // left
                if (needsRender(chunk, x - 1, y, z)) {
                    // vertices.append(v(4, 0, 3, x, y, z));
                    // vertices.append(v(3, 7, 4, x, y, z));
                }
                // top
                if (needsRender(chunk, x, y + 1, z)) {
                    // vertices.append(v(3, 2, 6, x, y, z));
                    // vertices.append(v(6, 7, 3, x, y, z));
                }
                // bottom
                if (needsRender(chunk, x, y - 1, z)) {
                    // vertices.append(v(4, 5, 1, x, y, z));
                    // vertices.append(v(1, 0, 4, x, y, z));
                }

                /*

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

    auto renderChunk = RenderChunk(vertices);
    renderChunk.setupRenderData();
    return renderChunk;
}
