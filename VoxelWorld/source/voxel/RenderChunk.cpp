#include "voxel/RenderChunk.h"

RenderChunk::RenderChunk(std::vector<Vertex> vertices) {
    this->vertices = vertices;
}

void RenderChunk::setupRenderData() {
    glGenVertexArrays(1, &vao); // one attribute
    glBindVertexArray(vao);

    // load vertex positions
    glGenBuffers(1, &vbo); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    // load texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texturePosition));
}

void RenderChunk::render(bool wireframe) {
    glBindVertexArray(this->vao);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(vertices.size()));
}
