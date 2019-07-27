#ifndef RENDER_CHUNK_GENERATOR_H
#define RENDER_CHUNK_GENERATOR_H

#include <GL/glew.h>

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
    RenderChunk fromChunk(const WorldGenerator& worldGenerator, const Chunk& chunk);

private:
    std::vector<Vertex> cubeMesh;
};

#endif // !RENDER_CHUNK_GENERATOR_H
