#include "voxel/RenderChunkGenerator.h"
#include "voxel/WorldGenerator.h"

RenderChunkGenerator::RenderChunkGenerator() {
    cubeMesh = std::make_shared<Mesh>(Mesh::loadFromFile("block.obj"));
}

static bool needsRender(const Chunk& chunk, const int x, const int y, const int z) {
    if (x < 0 || y < 0 || z < 0) {
        return true;
    }
    if (x >= CHUNK_SIZE || y >= CHUNK_SIZE || z >= CHUNK_SIZE) {
        return true;
    }
    return chunk(x, y, z) == BLOCK_AIR;
}

RenderChunk RenderChunkGenerator::fromChunk(const WorldGenerator& worldGenerator, const Chunk& chunk) {
    std::vector<Vertex> vs;

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk(x, y, z) == BLOCK_AIR) {
                    continue;
                }

                const auto o = glm::vec3(x, y, z);

                // top
                if (needsRender(chunk, x, y + 1, z)) {
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, -1.0f) + o, glm::vec2(0.313589f, 0.600387f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, 0.0f) + o, glm::vec2(0.313589f, 0.312806f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f) + o, glm::vec2(0.601170f, 0.312806f)));

                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, -1.0f) + o, glm::vec2(0.313589f, 0.600387f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f) + o, glm::vec2(0.601170f, 0.312806f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, -1.0f) + o, glm::vec2(0.601170f, 0.600387f)));
                }
                // bottom
                if (needsRender(chunk, x, y - 1, z)) {
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, -1.0f) + o, glm::vec2(0.927016f, 0.650563f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, 0.0f) + o, glm::vec2(0.927016f, 0.938145f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f) + o, glm::vec2(0.639435f, 0.938145f)));

                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, -1.0f) + o, glm::vec2(0.927016f, 0.650563f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f) + o, glm::vec2(0.639435f, 0.938145f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, -1.0f) + o, glm::vec2(0.639435f, 0.650563f)));
                }
                // right
                if (needsRender(chunk, x + 1, y, z)) {
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, -1.0f) + o, glm::vec2(0.602356f, 0.600009f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f) + o, glm::vec2(0.602502f, 0.312428f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, 0.0f) + o, glm::vec2(0.890083f, 0.312574f)));

                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, -1.0f) + o, glm::vec2(0.602356f, 0.600009f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, 0.0f) + o, glm::vec2(0.890083f, 0.312574f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, -1.0f) + o, glm::vec2(0.889937f, 0.600155f)));
                }
                // left
                if (needsRender(chunk, x - 1, y, z)) {
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, -1.0f) + o, glm::vec2(0.025300f, 0.601718f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f) + o, glm::vec2(0.025154f, 0.314136f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, 0.0f) + o, glm::vec2(0.312735f, 0.313990f)));

                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, -1.0f) + o, glm::vec2(0.025300f, 0.601718f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, 0.0f) + o, glm::vec2(0.312735f, 0.313990f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, -1.0f) + o, glm::vec2(0.312881f, 0.601572f)));
                }
                // front
                if (needsRender(chunk, x, y, z + 1)) {
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, 0.0f) + o, glm::vec2(0.313669f, 0.311237f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, 0.0f) + o, glm::vec2(0.314044f, 0.023656f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, 0.0f) + o, glm::vec2(0.601625f, 0.024030f)));

                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, 0.0f) + o, glm::vec2(0.313669f, 0.311237f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, 0.0f) + o, glm::vec2(0.601625f, 0.024030f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, 0.0f) + o, glm::vec2(0.601250f, 0.311611f)));
                }
                // back
                if (needsRender(chunk, x, y, z - 1)) {
                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, -1.0f) + o, glm::vec2(0.313199f, 0.889921f)));
                    vs.push_back(Vertex(glm::vec3(0.0f, 1.0f, -1.0f) + o, glm::vec2(0.313199f, 0.602340f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, -1.0f) + o, glm::vec2(0.600780f, 0.602340f)));

                    vs.push_back(Vertex(glm::vec3(0.0f, 0.0f, -1.0f) + o, glm::vec2(0.313199f, 0.889921f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 1.0f, -1.0f) + o, glm::vec2(0.600780f, 0.602340f)));
                    vs.push_back(Vertex(glm::vec3(1.0f, 0.0f, -1.0f) + o, glm::vec2(0.600780f, 0.889921f)));
                }
            }
        }
    }

    auto renderChunk = RenderChunk(vs);
    renderChunk.setupRenderData();
    return renderChunk;
}
