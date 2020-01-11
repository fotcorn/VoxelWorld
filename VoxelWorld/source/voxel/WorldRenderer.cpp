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

    // cache size is 10x the chunks that is visible at once
    int cacheSize = (CAMERA_CHUNK_DISTANCE * 2) * (CAMERA_CHUNK_DISTANCE * 2) * 10;
    renderChunkGenerator = std::make_shared<RenderChunkGenerator>(cacheSize);

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh.vert", Shader::Type::Vertex);
    this->shaderProgram.attachShader(vertexShader);
    this->shaderProgram.attachShader(fragmentShader);
    this->shaderProgram.setAttribLocation("vertex_position", 0);
    this->shaderProgram.setAttribLocation("texture_coordinate", 1);
    this->shaderProgram.link();
}

bool intersect(const Ray& ray, const glm::vec3 bounds[2]) {
    float tmin = (bounds[ray.sign[0]].x - ray.origin.x) * ray.inverseDirection.x;
    float tmax = (bounds[1 - ray.sign[0]].x - ray.origin.x) * ray.inverseDirection.x;
    float tymin = (bounds[ray.sign[1]].y - ray.origin.y) * ray.inverseDirection.y;
    float tymax = (bounds[1 - ray.sign[1]].y - ray.origin.y) * ray.inverseDirection.y;

    if ((tmin > tymax) || (tymin > tmax)) {
        return false;
    }
    if (tymin > tmin) {
        tmin = tymin;
    }
    if (tymax < tmax) {
        tmax = tymax;
    }

    float tzmin = (bounds[ray.sign[2]].z - ray.origin.z) * ray.inverseDirection.z;
    float tzmax = (bounds[1 - ray.sign[2]].z - ray.origin.z) * ray.inverseDirection.z;

    if ((tmin > tzmax) || (tzmin > tmax)) {
        return false;
    }
    return true;
}

void WorldRenderer::calculateSelectedChunk(glm::vec3 cameraPos, glm::vec3 cameraFront) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);
    auto ray = Ray(cameraPos, cameraFront);

    glm::vec3 bounds[2];

    selectedChunkPosition.reset();
    selectedBlockPosition.reset();
    float minimalDistance = std::numeric_limits<float>::max();

    for (int x = currentX - CAMERA_CHUNK_DISTANCE; x < currentX + CAMERA_CHUNK_DISTANCE; x++) {
        for (int z = currentZ - CAMERA_CHUNK_DISTANCE; z < currentZ + CAMERA_CHUNK_DISTANCE; z++) {
            const auto position = glm::ivec3(x, 1, z);
            const auto floatPosition = glm::vec3(position);

            bounds[0] = floatPosition;
            bounds[1] = floatPosition + glm::vec3(1.0f, 4.0f, 1.0f);

            if (intersect(ray, bounds)) {
                auto chunk = worldGenerator.getChunk(position);
                for (int x = 0; x < CHUNK_SIZE; x++) {
                    for (int y = 0; y < CHUNK_HEIGHT; y++) {
                        for (int z = 0; z < CHUNK_SIZE; z++) {
                            if ((*chunk)(x, y, z) == BLOCK_AIR) {
                                continue;
                            }

                            bounds[0] = floatPosition +
                                        glm::vec3(float(x) / CHUNK_SIZE, float(y) / CHUNK_SIZE, float(z) / CHUNK_SIZE);
                            bounds[1] =
                                floatPosition + glm::vec3(float(x + 1.0f) / CHUNK_SIZE, float(y + 1.0f) / CHUNK_SIZE,
                                                          float(z + 1.0f) / CHUNK_SIZE);
                            if (intersect(ray, bounds)) {
                                const float distance = glm::distance(bounds[0], cameraPos);
                                if (distance < minimalDistance) {
                                    selectedChunkPosition = std::make_shared<glm::ivec3>(position);
                                    selectedBlockPosition = std::make_shared<glm::ivec3>(x, y, z);
                                    minimalDistance = distance;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void WorldRenderer::addBlock() {
    if (selectedChunkPosition) {
        auto chunk = worldGenerator.getChunk(*selectedChunkPosition);
        if ((*chunk)(selectedBlockPosition->x, selectedBlockPosition->y + 1, selectedBlockPosition->z) == BLOCK_AIR) {
            (*chunk)(selectedBlockPosition->x, selectedBlockPosition->y + 1, selectedBlockPosition->z) =
                TextureAtlas::GROUND_MUD;
            chunk->dirty = true;
        }
    }
}

void WorldRenderer::removeBlock() {
    if (selectedChunkPosition) {
        auto chunk = worldGenerator.getChunk(*selectedChunkPosition);
        (*chunk)(selectedBlockPosition->x, selectedBlockPosition->y, selectedBlockPosition->z) = BLOCK_AIR;
    }
}

void WorldRenderer::render(glm::mat4 vp, glm::vec3 cameraPos, glm::vec3 cameraFront, bool wireframe) {
    const int currentX = int(cameraPos.x);
    const int currentZ = int(cameraPos.z);

    this->shaderProgram.use();
    this->texture->bind();

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

            auto chunk = worldGenerator.getChunk(position);

            std::shared_ptr<RenderChunk> renderChunk;
            if (selectedChunkPosition && *selectedChunkPosition.get() == position) {
                Chunk modifiedChunk(*chunk);
                modifiedChunk(selectedBlockPosition->x, selectedBlockPosition->y, selectedBlockPosition->z) =
                    TextureAtlas::WALL_BRICK_05;
                renderChunk = renderChunkGenerator->fromChunk(position, modifiedChunk, worldGenerator, false);
            } else {
                renderChunk = renderChunkGenerator->fromChunk(position, (*chunk), worldGenerator, true);
            }

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(floatPosition));
            auto mvp = vp * modelMatrix;
            this->shaderProgram.setUniform("mvp", mvp);
            renderChunk->render(wireframe);
        }
    }
}
