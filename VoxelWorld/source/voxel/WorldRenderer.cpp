#include "voxel/WorldRenderer.h"
#include "TextureAtlas.h"
#include "voxel/Ray.h"
#include "voxel/RenderChunkGenerator.h"

#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <limits>

const int CAMERA_CHUNK_DISTANCE = 15;

void WorldRenderer::init() {
    texture = std::make_shared<Texture>(Texture::loadFromFile("texture_atlas.gif"));
    renderChunkGenerator = std::make_shared<RenderChunkGenerator>(1000);

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);
    this->shaderProgram.setAttribLocation("vertex_position", 0);
    this->shaderProgram.setAttribLocation("texture_coordinate", 1);
    this->shaderProgram.link();
}

std::shared_ptr<glm::ivec3> WorldRenderer::calculateSelectedChunk(glm::vec3 cameraPos, glm::vec3 cameraFront) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);
    auto ray = Ray(cameraPos, cameraFront);

    std::vector<glm::ivec3> intersectionChunks;

    float tmin, tmax, tymin, tymax, tzmin, tzmax;
    glm::vec3 bounds[2];

    for (int x = currentX - CAMERA_CHUNK_DISTANCE; x < currentX + CAMERA_CHUNK_DISTANCE; x++) {
        for (int z = currentZ - CAMERA_CHUNK_DISTANCE; z < currentZ + CAMERA_CHUNK_DISTANCE; z++) {
            const auto position = glm::ivec3(x, 1, z);
            const auto floatPosition = glm::vec3(position);

            bounds[0] = floatPosition;
            bounds[1] = floatPosition + glm::vec3(1.0f, 1.0f, 1.0f);

            tmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.inverseDirection.x;
            tmax = (bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.inverseDirection.x;
            tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.inverseDirection.y;
            tymax = (bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.inverseDirection.y;

            if ((tmin > tymax) || (tymin > tmax)) {
                continue;
            }
            if (tymin > tmin) {
                tmin = tymin;
            }
            if (tymax < tmax) {
                tmax = tymax;
            }

            tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.inverseDirection.z;
            tzmax = (bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.inverseDirection.z;

            if ((tmin > tzmax) || (tzmin > tmax)) {
                continue;
            }
            intersectionChunks.push_back(position);
        }
    }

    std::shared_ptr<glm::ivec3> nearestChunk;
    if (intersectionChunks.size() != 0) {
        float minDistance = std::numeric_limits<float>::max();
        for (auto& chunk : intersectionChunks) {
            const float distance = glm::distance(glm::vec3(chunk), cameraPos);
            if (distance < minDistance) {
                minDistance = distance;
                nearestChunk = std::make_shared<glm::ivec3>(chunk);
            }
        }
    }
    return nearestChunk;
}

void WorldRenderer::render(glm::mat4 vp, glm::vec3 cameraPos, glm::vec3 cameraFront, bool wireframe) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);

    this->shaderProgram.use();
    this->texture->bind();

    const auto selectedChunk = calculateSelectedChunk(cameraPos, cameraFront);

    for (int x = currentX - CAMERA_CHUNK_DISTANCE; x < currentX + CAMERA_CHUNK_DISTANCE; x++) {
        for (int z = currentZ - CAMERA_CHUNK_DISTANCE; z < currentZ + CAMERA_CHUNK_DISTANCE; z++) {
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

            if (selectedChunk && *selectedChunk.get() == position) {
                continue;
            }

            const auto chunk = worldGenerator.getChunk(position);
            const auto renderChunk = renderChunkGenerator->fromChunk(position, (*chunk), worldGenerator);

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(floatPosition));
            auto mvp = vp * modelMatrix;
            this->shaderProgram.setUniform("mvp", mvp);
            renderChunk->render(wireframe);
        }
    }
}
