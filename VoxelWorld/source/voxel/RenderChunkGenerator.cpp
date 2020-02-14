#include "voxel/RenderChunkGenerator.h"
#include "TextureAtlas.h"
#include "voxel/World.h"

RenderChunkGenerator::RenderChunkGenerator(std::size_t cacheSize) : chunkCache(cacheSize) {
    textureAtlasSize = static_cast<float>(TEXTURE_ATLAS_SIZE);
    selectedBlockTextureOffset =
        glm::vec2(static_cast<float>(TextureAtlas::WALL_BRICK_05 - 1) / textureAtlasSize, 0.0f);

    const float tas = textureAtlasSize;
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

namespace {
bool needsRender(const Chunk& chunk, const int x, const int y, const int z, const glm::ivec3& position, World& world) {

    if (x < 0) {
        const auto adjacentChunk = world.getChunk(position + glm::ivec3(-1, 0, 0));
        return (*adjacentChunk)(CHUNK_SIZE - 1, y, z) == BLOCK_AIR;
    }
    if (y < 0) {
        // never render a block below lowest point
        return false;
    }
    if (z < 0) {
        const auto adjacentChunk = world.getChunk(position + glm::ivec3(0, 0, -1));
        return (*adjacentChunk)(x, y, CHUNK_SIZE - 1) == BLOCK_AIR;
    }

    if (x == CHUNK_SIZE) {
        const auto adjacentChunk = world.getChunk(position + glm::ivec3(1, 0, 0));
        return (*adjacentChunk)(0, y, z) == BLOCK_AIR;
    }
    if (y == CHUNK_HEIGHT) {
        // there will never be a block heigher than this, always render
        return true;
    }
    if (z == CHUNK_SIZE) {
        const auto adjacentChunk = world.getChunk(position + glm::ivec3(0, 0, 1));
        return (*adjacentChunk)(x, y, 0) == BLOCK_AIR;
    }
    return chunk(x, y, z) == BLOCK_AIR;
}
} // namespace

std::shared_ptr<RenderChunk> RenderChunkGenerator::fromChunk(const glm::ivec3& position, Chunk& chunk, World& world) {
    if (!chunk.changed && !chunk.tempChanged) {
        auto cacheEntry = chunkCache.get(position);
        if (cacheEntry) {
            return cacheEntry;
        }
    }

    std::vector<Vertex> vs;

    bool selectedBlockInChunk = world.selectedChunkPosition == position;

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

                bool thisBlockSelected = selectedBlockInChunk && world.selectedBlockPosition == glm::ivec3(x, y, z);

                // top
                if (needsRender(chunk, x, y + 1, z, position, world)) {
                    glm::vec2 renderTCO;
                    if (thisBlockSelected && world.selectedBlockSide == Side::TOP) {
                        renderTCO = selectedBlockTextureOffset;
                    } else {
                        renderTCO = tco;
                    }
                    vs.push_back(Vertex((cubeMesh[0].position + po) / cs, cubeMesh[0].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[1].position + po) / cs, cubeMesh[1].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[2].position + po) / cs, cubeMesh[2].texturePosition + renderTCO));

                    vs.push_back(Vertex((cubeMesh[3].position + po) / cs, cubeMesh[3].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[4].position + po) / cs, cubeMesh[4].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[5].position + po) / cs, cubeMesh[5].texturePosition + renderTCO));
                }

                // bottom
                if (needsRender(chunk, x, y - 1, z, position, world)) {
                    glm::vec2 renderTCO;
                    if (thisBlockSelected && world.selectedBlockSide == Side::BOTTOM) {
                        renderTCO = selectedBlockTextureOffset;
                    } else {
                        renderTCO = tco;
                    }
                    vs.push_back(Vertex((cubeMesh[6].position + po) / cs, cubeMesh[6].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[7].position + po) / cs, cubeMesh[7].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[8].position + po) / cs, cubeMesh[8].texturePosition + renderTCO));

                    vs.push_back(Vertex((cubeMesh[9].position + po) / cs, cubeMesh[9].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[10].position + po) / cs, cubeMesh[10].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[11].position + po) / cs, cubeMesh[11].texturePosition + renderTCO));
                }

                // right
                if (needsRender(chunk, x + 1, y, z, position, world)) {
                    glm::vec2 renderTCO;
                    if (thisBlockSelected && world.selectedBlockSide == Side::RIGHT) {
                        renderTCO = selectedBlockTextureOffset;
                    } else {
                        renderTCO = tco;
                    }
                    vs.push_back(Vertex((cubeMesh[12].position + po) / cs, cubeMesh[12].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[13].position + po) / cs, cubeMesh[13].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[14].position + po) / cs, cubeMesh[14].texturePosition + renderTCO));

                    vs.push_back(Vertex((cubeMesh[15].position + po) / cs, cubeMesh[15].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[16].position + po) / cs, cubeMesh[16].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[17].position + po) / cs, cubeMesh[17].texturePosition + renderTCO));
                }

                // left
                if (needsRender(chunk, x - 1, y, z, position, world)) {
                    glm::vec2 renderTCO;
                    if (thisBlockSelected && world.selectedBlockSide == Side::LEFT) {
                        renderTCO = selectedBlockTextureOffset;
                    } else {
                        renderTCO = tco;
                    }
                    vs.push_back(Vertex((cubeMesh[18].position + po) / cs, cubeMesh[18].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[19].position + po) / cs, cubeMesh[19].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[20].position + po) / cs, cubeMesh[20].texturePosition + renderTCO));

                    vs.push_back(Vertex((cubeMesh[21].position + po) / cs, cubeMesh[21].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[22].position + po) / cs, cubeMesh[22].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[23].position + po) / cs, cubeMesh[23].texturePosition + renderTCO));
                }

                // front
                if (needsRender(chunk, x, y, z + 1, position, world)) {
                    glm::vec2 renderTCO;
                    if (thisBlockSelected && world.selectedBlockSide == Side::FRONT) {
                        renderTCO = selectedBlockTextureOffset;
                    } else {
                        renderTCO = tco;
                    }
                    vs.push_back(Vertex((cubeMesh[24].position + po) / cs, cubeMesh[24].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[25].position + po) / cs, cubeMesh[25].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[26].position + po) / cs, cubeMesh[26].texturePosition + renderTCO));

                    vs.push_back(Vertex((cubeMesh[27].position + po) / cs, cubeMesh[27].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[28].position + po) / cs, cubeMesh[28].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[29].position + po) / cs, cubeMesh[29].texturePosition + renderTCO));
                }

                // back
                if (needsRender(chunk, x, y, z - 1, position, world)) {
                    glm::vec2 renderTCO;
                    if (thisBlockSelected && world.selectedBlockSide == Side::BACK) {
                        renderTCO = selectedBlockTextureOffset;
                    } else {
                        renderTCO = tco;
                    }
                    vs.push_back(Vertex((cubeMesh[30].position + po) / cs, cubeMesh[30].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[31].position + po) / cs, cubeMesh[31].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[32].position + po) / cs, cubeMesh[32].texturePosition + renderTCO));

                    vs.push_back(Vertex((cubeMesh[33].position + po) / cs, cubeMesh[33].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[34].position + po) / cs, cubeMesh[34].texturePosition + renderTCO));
                    vs.push_back(Vertex((cubeMesh[35].position + po) / cs, cubeMesh[35].texturePosition + renderTCO));
                }
            }
        }
    }

    auto renderChunk = std::make_shared<RenderChunk>(vs);
    if (!chunk.tempChanged) {
        chunkCache.set(position, renderChunk);
    }
    if (chunk.tempChanged && chunk.changed) {
        // remove old cached RenderChunks when a block was changed and a block was selected at the same time.
        // if we do not do this, old RenderChunks can show up when no block is selected in the same chunk any more:
        // => chunk.tempChanged and chunk.changed are both false, which means this function will use old RenderChunks
        // from the cache which might not have all changed blocks in it.
        chunkCache.remove(position);
    }
    chunk.changed = false;
    chunk.tempChanged = false;
    return renderChunk;
}
