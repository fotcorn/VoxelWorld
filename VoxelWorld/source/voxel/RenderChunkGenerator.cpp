#include "voxel/RenderChunkGenerator.h"
#include "TextureAtlas.h"
#include "voxel/WorldGenerator.h"

RenderChunkGenerator::RenderChunkGenerator(std::size_t cacheSize) : chunkCache(cacheSize) {
    const float tas = static_cast<float>(TEXTURE_ATLAS_SIZE);
    cubeMesh = {// top
                Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.313589f / tas, 0.600387f)),
                Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.313589f / tas, 0.312806f)),
                Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.601170f / tas, 0.312806f)),

                Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.313589f / tas, 0.600387f)),
                Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.601170f / tas, 0.312806f)),
                Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.601170f / tas, 0.600387f)),

                // bottom
                Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.927016f / tas, 0.650563f)),
                Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.927016f / tas, 0.938145f)),
                Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.639435f / tas, 0.938145f)),

                Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.927016f / tas, 0.650563f)),
                Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.639435f / tas, 0.938145f)),
                Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.639435f / tas, 0.650563f)),

                // right
                Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.602356f / tas, 0.600009f)),
                Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.602502f / tas, 0.312428f)),
                Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.890083f / tas, 0.312574f)),

                Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.602356f / tas, 0.600009f)),
                Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.890083f / tas, 0.312574f)),
                Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.889937f / tas, 0.600155f)),

                // left
                Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.025300f / tas, 0.601718f)),
                Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.025154f / tas, 0.314136f)),
                Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.312735f / tas, 0.313990f)),

                Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.025300f / tas, 0.601718f)),
                Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.312735f / tas, 0.313990f)),
                Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.312881f / tas, 0.601572f)),

                // front
                Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.313669f / tas, 0.311237f)),
                Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.314044f / tas, 0.023656f)),
                Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.601625f / tas, 0.024030f)),

                Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.313669f / tas, 0.311237f)),
                Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.601625f / tas, 0.024030f)),
                Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(0.601250f / tas, 0.311611f)),

                // back
                Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.313199f / tas, 0.889921f)),
                Vertex(glm::vec3(0.0f, 1.0f, -1.0f), glm::vec2(0.313199f / tas, 0.602340f)),
                Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.600780f / tas, 0.602340f)),

                Vertex(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.313199f / tas, 0.889921f)),
                Vertex(glm::vec3(1.0f, 1.0f, -1.0f), glm::vec2(0.600780f / tas, 0.602340f)),
                Vertex(glm::vec3(1.0f, 0.0f, -1.0f), glm::vec2(0.600780f / tas, 0.889921f))};
}

static bool needsRender(const Chunk& chunk, const int x, const int y, const int z, const glm::ivec3& position,
                        WorldGenerator& worldGenerator) {

    if (x < 0) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(-1, 0, 0));
        return (*adjacentChunk)(CHUNK_SIZE - 1, y, z) == BLOCK_AIR;
    }
    if (y < 0) {
        // never render a block below lowest point
        return false;
    }
    if (z < 0) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(0, 0, -1));
        return (*adjacentChunk)(x, y, CHUNK_SIZE - 1) == BLOCK_AIR;
    }

    if (x == CHUNK_SIZE) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(1, 0, 0));
        return (*adjacentChunk)(0, y, z) == BLOCK_AIR;
    }
    if (y == CHUNK_HEIGHT) {
        // there will never be a block heigher than this, always render
        return true;
    }
    if (z == CHUNK_SIZE) {
        const auto adjacentChunk = worldGenerator.getChunk(position + glm::ivec3(0, 0, 1));
        return (*adjacentChunk)(x, y, 0) == BLOCK_AIR;
    }
    return chunk(x, y, z) == BLOCK_AIR;
}

const std::shared_ptr<RenderChunk> RenderChunkGenerator::fromChunk(const glm::ivec3 position, Chunk& chunk,
                                                                   WorldGenerator& worldGenerator, bool useCache) {
    if (!chunk.dirty && useCache) {
        auto cacheEntry = chunkCache.get(position);
        if (cacheEntry) {
            return cacheEntry;
        }
    }

    std::vector<Vertex> vs;

    const float textureAtlasSize = static_cast<float>(TEXTURE_ATLAS_SIZE);

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk(x, y, z) == BLOCK_AIR) {
                    continue;
                }

                // position offset
                const auto po = glm::vec3(x, y, z);

                // texture coordinate offset
                const auto tco = glm::vec2(static_cast<float>(chunk(x, y, z) - 1) / textureAtlasSize, 0.0f);

                // chunk size
                const float cs = static_cast<float>(CHUNK_SIZE);

                // top
                if (needsRender(chunk, x, y + 1, z, position, worldGenerator)) {
                    vs.push_back(Vertex((cubeMesh[0].position + po) / cs, cubeMesh[0].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[1].position + po) / cs, cubeMesh[1].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[2].position + po) / cs, cubeMesh[2].texturePosition + tco));

                    vs.push_back(Vertex((cubeMesh[3].position + po) / cs, cubeMesh[3].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[4].position + po) / cs, cubeMesh[4].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[5].position + po) / cs, cubeMesh[5].texturePosition + tco));
                }

                // bottom
                if (needsRender(chunk, x, y - 1, z, position, worldGenerator)) {
                    vs.push_back(Vertex((cubeMesh[6].position + po) / cs, cubeMesh[6].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[7].position + po) / cs, cubeMesh[7].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[8].position + po) / cs, cubeMesh[8].texturePosition + tco));

                    vs.push_back(Vertex((cubeMesh[9].position + po) / cs, cubeMesh[9].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[10].position + po) / cs, cubeMesh[10].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[11].position + po) / cs, cubeMesh[11].texturePosition + tco));
                }

                // right
                if (needsRender(chunk, x + 1, y, z, position, worldGenerator)) {
                    vs.push_back(Vertex((cubeMesh[12].position + po) / cs, cubeMesh[12].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[13].position + po) / cs, cubeMesh[13].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[14].position + po) / cs, cubeMesh[14].texturePosition + tco));

                    vs.push_back(Vertex((cubeMesh[15].position + po) / cs, cubeMesh[15].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[16].position + po) / cs, cubeMesh[16].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[17].position + po) / cs, cubeMesh[17].texturePosition + tco));
                }

                // left
                if (needsRender(chunk, x - 1, y, z, position, worldGenerator)) {
                    vs.push_back(Vertex((cubeMesh[18].position + po) / cs, cubeMesh[18].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[19].position + po) / cs, cubeMesh[19].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[20].position + po) / cs, cubeMesh[20].texturePosition + tco));

                    vs.push_back(Vertex((cubeMesh[21].position + po) / cs, cubeMesh[21].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[22].position + po) / cs, cubeMesh[22].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[23].position + po) / cs, cubeMesh[23].texturePosition + tco));
                }

                // front
                if (needsRender(chunk, x, y, z + 1, position, worldGenerator)) {
                    vs.push_back(Vertex((cubeMesh[24].position + po) / cs, cubeMesh[24].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[25].position + po) / cs, cubeMesh[25].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[26].position + po) / cs, cubeMesh[26].texturePosition + tco));

                    vs.push_back(Vertex((cubeMesh[27].position + po) / cs, cubeMesh[27].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[28].position + po) / cs, cubeMesh[28].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[29].position + po) / cs, cubeMesh[29].texturePosition + tco));
                }

                // back
                if (needsRender(chunk, x, y, z - 1, position, worldGenerator)) {
                    vs.push_back(Vertex((cubeMesh[30].position + po) / cs, cubeMesh[30].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[31].position + po) / cs, cubeMesh[31].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[32].position + po) / cs, cubeMesh[32].texturePosition + tco));

                    vs.push_back(Vertex((cubeMesh[33].position + po) / cs, cubeMesh[33].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[34].position + po) / cs, cubeMesh[34].texturePosition + tco));
                    vs.push_back(Vertex((cubeMesh[35].position + po) / cs, cubeMesh[35].texturePosition + tco));
                }
            }
        }
    }

    auto renderChunk = std::make_shared<RenderChunk>(vs);
    if (useCache) {
        chunkCache.set(position, renderChunk);
    }
    chunk.dirty = false;
    return renderChunk;
}
