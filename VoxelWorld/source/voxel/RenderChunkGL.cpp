#include "voxel/RenderChunk.h"

void RenderChunk::setupRenderData() {
    // TODO
    /*
    // Object
    glGenVertexArrays(1, &this->vertexAttributeObject); // one attribute
    glBindVertexArray(this->vertexAttributeObject);

    GLuint vertexPositions = 0;
    glGenBuffers(1, &vertexPositions); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositions); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vertexPositions);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    GLuint ebo = 0;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::uvec3) * indices.size(), &indices[0], GL_STATIC_DRAW);

    this->incidesCount = indices.size() * 3;
    */

    /*
    // Mesh
    glGenVertexArrays(1, &model.vao); // one attribute
    glBindVertexArray(model.vao);

    // load vertex positions
    glGenBuffers(1, &model.vbo); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * model.vertices.size(), &model.vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, model.vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

     // load vertex incides
     glGenBuffers(1, &model.ebo);
     glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model.ebo);
     glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * model.indices.size(), &model.indices[0],
                  GL_STATIC_DRAW);

     // load texture coordinates
     glEnableVertexAttribArray(1);
     glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texturePosition));
     */
}

void RenderChunk::render(bool wireframe) {
    glBindVertexArray(this->vao);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr);
}
