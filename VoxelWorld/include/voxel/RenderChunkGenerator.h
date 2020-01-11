#ifndef RENDER_CHUNK_GENERATOR_H
#define RENDER_CHUNK_GENERATOR_H

#include <GL/glew.h>

#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

#include "LimitedUnorderedMap.h"
#include "Mesh.h"
#include "Vertex.h"
#include "voxel/RenderChunk.h"
#include "voxel/WorldGenerator.h"

class RenderChunkGenerator {
public:
    RenderChunkGenerator(std::size_t cacheSize);
    const std::shared_ptr<RenderChunk> fromChunk(const glm::ivec3 position, Chunk& chunk,
                                                 WorldGenerator& worldGenerator, bool useCache);

private:
    std::vector<Vertex> cubeMesh;
    LimitedUnorderedMap<glm::ivec3, RenderChunk> chunkCache;
};

#endif // !RENDER_CHUNK_GENERATOR_H
