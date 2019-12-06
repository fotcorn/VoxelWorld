
#ifndef RENDER_CHUNK_H
#define RENDER_CHUNK_H

#include <GL/glew.h>

#include <vector>

#include "Vertex.h"

class RenderChunk {
public:
    RenderChunk() {
    }
    RenderChunk(const std::vector<Vertex>& vertices);
    void render(const bool wireframe);

private:
    GLuint vao;
    GLuint vbo;
    uint32_t verticesCount;
};

#endif // !RENDER_CHUNK_H
