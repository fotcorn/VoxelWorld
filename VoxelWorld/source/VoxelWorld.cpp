#include "VoxelWorld.h"

#include <iostream>
#include <memory>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>

#include "Mesh.h"
#include "PerlinNoise.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Tensor3.h"
#include "Texture.h"

const int BLOCK_AIR = 0;

const int WORLD_X = 100;
const int WORLD_Y = 30;
const int WORLD_Z = 100;

const int NOISE_SCALE = 50;

const int WATER_HEIGHT = 6;

using WorldTensor = Tensor3<char, WORLD_X, WORLD_Y, WORLD_Z>;

static bool needsRender(const std::shared_ptr<WorldTensor>& world, const int x, const int y, const int z) {
    if (x < 0 || y < 0 || z < 0) {
        return false;
    }
    if (x >= WORLD_X || y >= WORLD_Y || z >= WORLD_Z) {
        return true;
    }
    return (*world)(x, y, z) == BLOCK_AIR;
}

static bool isVisible(const std::shared_ptr<WorldTensor>& world, const int x, const int y, const int z) {
    return needsRender(world, x - 1, y, z) || needsRender(world, x + 1, y, z) || needsRender(world, x, y - 1, z) ||
           needsRender(world, x, y + 1, z) || needsRender(world, x, y, z - 1) || needsRender(world, x, y, z + 1);
}

void VoxelWorld::init() {
    siv::PerlinNoise noise(1);

    auto world = std::make_shared<WorldTensor>();

    // basic world generation
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            const double value = noise.noise0_1(double(x) / double(NOISE_SCALE), double(z) / double(NOISE_SCALE));
            const int height = value * WORLD_Y;
            for (int y = 0; y < height; y++) {
                if (y > WORLD_Y * 0.7) {
                    (*world)(x, y, z) = TextureAtlas::SNOW;
                } else if (y > WORLD_Y * 0.5) {
                    (*world)(x, y, z) = TextureAtlas::STONE_04;
                } else {
                    (*world)(x, y, z) = TextureAtlas::GROUND_EARTH;
                }
            }
        }
    }

    // water
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            for (int y = 0; y <= WATER_HEIGHT; y++) {
                if ((*world)(x, y, z) == BLOCK_AIR) {
                    (*world)(x, y, z) = TextureAtlas::WATER;
                }
            }
            if ((*world)(x, WATER_HEIGHT, z) == TextureAtlas::GROUND_EARTH &&
                (*world)(x, WATER_HEIGHT + 1, z) == BLOCK_AIR) {
                (*world)(x, WATER_HEIGHT, z) = TextureAtlas::WATER;
            }
        }
    }

    block = std::make_shared<Mesh>(Mesh::loadFromFile("block.obj"));
    texture = std::make_shared<Texture>(Texture::loadFromFile("texture_atlas.gif"));

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh_atlas.vert", Shader::Type::Vertex);
    this->shaderProgram = std::make_shared<ShaderProgram>();
    this->shaderProgram->attachShader(vertexShader);
    this->shaderProgram->attachShader(fragmentShader);
    this->shaderProgram->setAttribLocation("vertex_position", 0);
    this->shaderProgram->setAttribLocation("texture_coordinate", 1);
    this->shaderProgram->link();

    for (int x = 0; x < WORLD_X; x++) {
        for (int y = 0; y < WORLD_Y; y++) {
            for (int z = 0; z < WORLD_Z; z++) {
                if ((*world)(x, y, z) == BLOCK_AIR) {
                    continue;
                }
                if (!isVisible(world, x, y, z)) {
                    continue;
                }

                glm::mat4 cubeModel = glm::mat4(1.0f);
                glm::vec3 cubePosition = glm::vec3(x, y, z);
                cubeModel = glm::translate(cubeModel, cubePosition);

                blocks.push_back({cubeModel, glm::ivec3(x, y, z), (TextureAtlas)(*world)(x, y, z)});
            }
        }
    }

    std::cerr << "Rendered blocks:" << blocks.size() << std::endl;
}

void VoxelWorld::render(glm::mat4 vp, glm::vec3 cameraPos, bool wireframe) {
    this->shaderProgram->use();
    this->texture->bind();
    for (const auto& block : blocks) {
        if (glm::distance(block.position, cameraPos) > 50.0) {
            continue;
        }

        auto mvp = vp * block.model;
        this->shaderProgram->setUniform("mvp", mvp);
        this->shaderProgram->setUniform("texture_offset", static_cast<int>(block.type) - 1);
        this->block->draw(wireframe);
    }
}
