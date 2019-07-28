#include "voxel/WorldRenderer.h"
#include "TextureAtlas.h"
#include "voxel/RenderChunkGenerator.h"

#include <glm/gtx/rotate_vector.hpp>

#include <iostream>

const int CAMERA_CHUNK_DISTANCE = 10;

void WorldRenderer::init() {
    texture = std::make_shared<Texture>(Texture::loadFromFile("texture_atlas.gif"));
    renderChunkGenerator = std::make_shared<RenderChunkGenerator>();

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);
    this->shaderProgram.setAttribLocation("vertex_position", 0);
    this->shaderProgram.setAttribLocation("texture_coordinate", 1);
    this->shaderProgram.link();
}

void WorldRenderer::render(glm::mat4 vp, glm::vec3 cameraPos, bool wireframe) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);

    this->shaderProgram.use();
    this->texture->bind();

    for (int x = currentX - CAMERA_CHUNK_DISTANCE; x < currentX + CAMERA_CHUNK_DISTANCE; x++) {
        for (int z = currentZ - CAMERA_CHUNK_DISTANCE; z < currentZ + CAMERA_CHUNK_DISTANCE; z++) {
            auto position = glm::ivec3(x, 1, z);
            auto chunk = worldGenerator.getChunk(position);
            auto renderChunk = renderChunkGenerator->fromChunk(position, chunk, worldGenerator);

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(position));
            float scale = 1.0f / float(CHUNK_SIZE);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(scale));
            auto mvp = vp * modelMatrix;
            this->shaderProgram.setUniform("mvp", mvp);
            renderChunk.render(wireframe);
        }
    }
}
