
#ifndef RENDER_CHUNK_H
#define RENDER_CHUNK_H

#include <GL/glew.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <memory>
#include <vector>

#include "Vertex.h"

class RenderChunk {
public:
    RenderChunk() {
    }
    RenderChunk(std::vector<Vertex> vertices);
    void render(bool wireframe);
    void setupRenderData();

private:
    std::vector<Vertex> vertices;

    GLuint vao;
    GLuint vbo;
};

#endif // !RENDER_CHUNK_H
