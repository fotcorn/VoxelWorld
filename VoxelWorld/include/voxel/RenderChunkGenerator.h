#ifndef RENDER_CHUNK_GENERATOR_H
#define RENDER_CHUNK_GENERATOR_H

#include <GL/glew.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

#include "Mesh.h"
#include "Vertex.h"
#include "voxel/RenderChunk.h"
#include "voxel/WorldGenerator.h"

class RenderChunkGenerator {
public:
    RenderChunkGenerator();
    RenderChunk fromChunk(const glm::ivec3 position, const Chunk& chunk, const WorldGenerator& worldGenerator);

private:
    std::vector<Vertex> cubeMesh;
    std::unordered_map<glm::ivec3, RenderChunk> chunkCache;
};

#endif // !RENDER_CHUNK_GENERATOR_H
