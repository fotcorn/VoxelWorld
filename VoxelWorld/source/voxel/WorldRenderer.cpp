#include "voxel/WorldRenderer.h"
#include "voxel/RenderChunk.h"

void WorldRenderer::init() {
    texture = Texture::loadFromFile("texture_atlas.gif");

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh_atlas.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);
    this->shaderProgram.setAttribLocation("vertex_position", 0);
    this->shaderProgram.setAttribLocation("texture_coordinate", 1);
    this->shaderProgram.link();
}

void WorldRenderer::render(glm::mat4 vp, glm::vec3, bool wireframe) {
    this->shaderProgram.use();
    this->texture.bind();

    auto chunk = worldGenerator.getChunk(1, 1, 1);
    auto renderChunk = RenderChunk::fromChunk(worldGenerator, chunk);

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(1, 1, 1);
    modelMatrix = glm::translate(modelMatrix, position);
    auto mvp = vp * modelMatrix;
    this->shaderProgram.setUniform("mvp", mvp);
    renderChunk.render(wireframe);
}
