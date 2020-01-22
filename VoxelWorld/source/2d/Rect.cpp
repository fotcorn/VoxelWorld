#include "2d/Rect.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/vec3.hpp>

#include <vector>

#include "Shader.h"

GLuint Rect::vao = 0;
GLuint Rect::vbo = 0;
std::shared_ptr<ShaderProgram> Rect::shaderProgram = std::shared_ptr<ShaderProgram>();

Rect::Rect(float x, float y, float z, float width, float height, glm::vec3 color)
    : x(x), y(y), z(z), width(width), height(height), color(color) {
    if (!Rect::shaderProgram) {
        Rect::init();
    }
}

void Rect::init() {
    // shader
    Shader fragmentShader = Shader::loadFromFile("rect.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("rect.vert", Shader::Type::Vertex);

    Rect::shaderProgram = std::make_shared<ShaderProgram>();
    Rect::shaderProgram->attachShader(vertexShader);
    Rect::shaderProgram->attachShader(fragmentShader);

    Rect::shaderProgram->setAttribLocation("vertex_position", 0);
    Rect::shaderProgram->setAttribLocation("color", 1);

    Rect::shaderProgram->link();

    std::vector<glm::vec3> vertices = {
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f),

    };

    // vao/vbo
    glGenVertexArrays(1, &Rect::vao); // one attribute
    glBindVertexArray(Rect::vao);

    // load vertex positions
    glGenBuffers(1, &Rect::vbo); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, Rect::vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
}

void Rect::render(glm::mat4 projectionMatrix, bool wireframe) {
    Rect::shaderProgram->use();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, 1.0f));

    auto mvp = projectionMatrix * modelMatrix;

    Rect::shaderProgram->setUniform("mvp", mvp);
    Rect::shaderProgram->setUniform("color", color);

    glBindVertexArray(Rect::vao);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(6));
}
