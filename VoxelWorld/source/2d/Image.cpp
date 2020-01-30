#include "2d/Image.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/vec3.hpp>

#include <vector>

#include "Shader.h"
#include "Vertex.h"

GLuint Image::vao = 0;
GLuint Image::vbo = 0;
std::shared_ptr<ShaderProgram> Image::shaderProgram = std::shared_ptr<ShaderProgram>();

Image::Image(float x, float y, float z, float width, float height, Texture texture)
    : x(x), y(y), z(z), width(width), height(height), texture(texture) {
    if (!Image::shaderProgram) {
        Image::init();
    }
}

void Image::init() {
    // shader
    Shader fragmentShader = Shader::loadFromFile("image.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("image.vert", Shader::Type::Vertex);

    Image::shaderProgram = std::make_shared<ShaderProgram>();
    Image::shaderProgram->attachShader(vertexShader);
    Image::shaderProgram->attachShader(fragmentShader);

    Image::shaderProgram->setAttribLocation("vertex_position", 0);
    Image::shaderProgram->setAttribLocation("color", 1);

    Image::shaderProgram->link();

    std::vector<Vertex> vertices = {Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
                                    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
                                    Vertex(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),

                                    Vertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),
                                    Vertex(glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
                                    Vertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f))};

    glGenVertexArrays(1, &Image::vao); // one attribute
    glBindVertexArray(Image::vao);

    // load vertex positions
    glGenBuffers(1, &Image::vbo); // one buffer in this vertex buffer object
    glBindBuffer(GL_ARRAY_BUFFER, Image::vbo); // set current buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0],
                 GL_STATIC_DRAW); // copy data to GPU memory

    glEnableVertexAttribArray(0); // activate first attribute
    glBindBuffer(GL_ARRAY_BUFFER, Image::vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

    // load texture coordinates
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texturePosition));
}

void Image::render(glm::mat4 projectionMatrix, bool wireframe) {
    Image::shaderProgram->use();

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(x, y, z));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(width, height, 1.0f));

    auto mvp = projectionMatrix * modelMatrix;

    Image::shaderProgram->setUniform("mvp", mvp);
    texture.bind();

    glBindVertexArray(Image::vao);
    glPolygonMode(GL_FRONT_AND_BACK, wireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(6));
}
