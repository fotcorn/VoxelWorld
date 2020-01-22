#include "voxel/WorldRenderer.h"
#include "TextureAtlas.h"
#include "voxel/Ray.h"
#include "voxel/RenderChunkGenerator.h"

#include <glm/gtx/rotate_vector.hpp>

#include <limits>

WorldRenderer::WorldRenderer(int cameraChunkDistance) {
    this->cameraChunkDistance = cameraChunkDistance;
    texture = std::make_shared<Texture>(Texture::loadFromFile("texture_atlas.gif"));

    // cache size is 10x the chunks that is visible at once
    int cacheSize = (cameraChunkDistance * 2) * (cameraChunkDistance * 2) * 10;
    renderChunkGenerator = std::make_shared<RenderChunkGenerator>(cacheSize);

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);
    this->shaderProgram.setAttribLocation("vertex_position", 0);
    this->shaderProgram.setAttribLocation("texture_coordinate", 1);
    this->shaderProgram.link();
}

void WorldRenderer::render(World& world, glm::mat4 vp, glm::vec3 cameraPos, glm::vec3 cameraFront, bool wireframe) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);

    this->shaderProgram.use();
    this->texture->bind();

    for (int x = currentX - cameraChunkDistance; x < currentX + cameraChunkDistance; x++) {
        for (int z = currentZ - cameraChunkDistance; z < currentZ + cameraChunkDistance; z++) {
            const auto position = glm::ivec3(x, 1, z);
            const auto floatPosition = glm::vec3(position);

            // check if current chunk is behind the camera, if yes, skip rendering it
            // vector from camera position to current block position
            const glm::vec3 cameraRelativeVector = floatPosition - cameraPos;
            // if dot product is smaller than 0.0 (angle between the two vectors is greater than perpendicular),
            // block is behind the camera
            if (glm::dot(cameraRelativeVector, cameraFront) < 0.0f) {
                continue;
            }

            auto chunk = world.getChunk(position);
            auto renderChunk = renderChunkGenerator->fromChunk(position, (*chunk), world);

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(floatPosition));
            auto mvp = vp * modelMatrix;
            this->shaderProgram.setUniform("mvp", mvp);
            renderChunk->render(wireframe);
        }
    }
}
