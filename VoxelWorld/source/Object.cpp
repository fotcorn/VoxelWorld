#include "Object.h"

Object::Object(std::vector<glm::vec3> vertices, std::vector<glm::uvec3> indices) {
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
}

Object::Object(std::vector<glm::vec3> vertices, std::vector<glm::uvec3> indices, std::vector<glm::vec3> colors)
    : Object(vertices, indices) {
    GLuint vertexColors = 0;
    glGenBuffers(1, &vertexColors); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vertexColors); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * colors.size(), &colors[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(1); // activate first attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
}

void Object::draw(bool wireframe) {
    glBindVertexArray(this->vertexAttributeObject);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawElements(GL_TRIANGLES, this->incidesCount, GL_UNSIGNED_INT, nullptr);
}
