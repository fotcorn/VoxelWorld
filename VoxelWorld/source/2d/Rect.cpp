#include "2d/Rect.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/vec3.hpp>

#include <iostream>
#include <vector>

#include "Shader.h"

Rect::Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {
    // shader
    Shader fragmentShader = Shader::loadFromFile("rect.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("rect.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);

    this->shaderProgram.setAttribLocation("vertex_position", 0);
    this->shaderProgram.setAttribLocation("color", 1);

    this->shaderProgram.link();

    std::vector<glm::vec3> vertices = {
        glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, -1.0f, 1.0f), glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f),
    };

    // vao/vbo
    glGenVertexArrays(1, &vao); // one attribute
    glBindVertexArray(vao);

    // load vertex positions
    glGenBuffers(1, &vbo); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
}

Rect::~Rect() {
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

void Rect::render(glm::mat4 projectionMatrix, bool wireframe) {
    this->shaderProgram.use();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(static_cast<float>(x), static_cast<float>(y), 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(static_cast<float>(width), static_cast<float>(height), 1.0f));

    auto mvp = projectionMatrix * modelMatrix;

    this->shaderProgram.setUniform("mvp", mvp);
    this->shaderProgram.setUniform("color", glm::ivec3(1.0f, 0.0f, 1.0f));

    glBindVertexArray(this->vao);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(6));
}
