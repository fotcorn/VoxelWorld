#ifndef RENDER_CHUNK_H
#define RENDER_CHUNK_H

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <vector>

#include "voxel/WorldGenerator.h"

struct Vertex {
    glm::vec3 position = glm::vec3();
    glm::vec2 texturePosition = glm::vec3();
};

class RenderChunk {
public:
    static RenderChunk fromChunk(const WorldGenerator& worldGenerator, const Chunk& chunk);
    void render(bool wireframe);

private:
    RenderChunk() = default;
    void setupRenderData();

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

#endif // !RENDER_CHUNK_H
