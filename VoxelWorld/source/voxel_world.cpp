#include "voxel_world.h"

#include <iostream>
#include <memory>
#include <unordered_map>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/mat4x4.hpp>

#include "mesh.h"
#include "perlin_noise.h"
#include "shader.h"
#include "shader_program.h"
#include "texture.h"

const char BLOCK_AIR = 0;
const char BLOCK_MUD = 1;
const char BLOCK_EARTH = 2;
const char BLOCK_SNOW = 3;
const char BLOCK_ROCK = 4;
const char BLOCK_WATER = 5;

const int WORLD_X = 500;
const int WORLD_Y = 30;
const int WORLD_Z = 500;

const int WATER_HEIGHT = 6;

static bool needsRender(char world[WORLD_X][WORLD_Y][WORLD_Z], int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0) {
        return false;
    } else if (x >= WORLD_X || y >= WORLD_Y || z >= WORLD_Z) {
        return true;
    }
    return world[x][y][z] == 0;
}

static bool isVisible(char world[WORLD_X][WORLD_Y][WORLD_Z], int x, int y, int z) {
    return needsRender(world, x - 1, y, z) || needsRender(world, x + 1, y, z) || needsRender(world, x, y - 1, z) ||
           needsRender(world, x, y + 1, z) || needsRender(world, x, y, z - 1) || needsRender(world, x, y, z + 1);
}

void VoxelWorld::init() {
    siv::PerlinNoise noise(1);

    char world[WORLD_X][WORLD_Y][WORLD_Z] = {{{BLOCK_AIR}}};

    // basic world generation
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            const double value = noise.noise0_1(double(x) / double(WORLD_X), double(z) / double(WORLD_Y)) * 1.6 - 0.4;
            const int height = value * WORLD_Y;
            for (int y = 0; y < height; y++) {
                if (y > WORLD_Y * 0.6) {
                    world[x][y][z] = BLOCK_SNOW;
                } else if (y > WORLD_Y * 0.3) {
                    world[x][y][z] = BLOCK_ROCK;
                } else {
                    world[x][y][z] = BLOCK_EARTH;
                }
            }
        }
    }

    // water
    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            for (int y = 0; y <= WATER_HEIGHT; y++) {
                if (world[x][y][z] == BLOCK_AIR) {
                    world[x][y][z] = BLOCK_WATER;
                }
            }
            if (world[x][WATER_HEIGHT][z] == BLOCK_EARTH && world[x][WATER_HEIGHT + 1][z] == BLOCK_AIR) {
                world[x][WATER_HEIGHT][z] = BLOCK_WATER;
            }
        }
    }

    block = std::make_shared<Mesh>(Mesh::loadFromFile("block.obj"));

    textureMap[BLOCK_MUD] = std::make_shared<Texture>(Texture::loadFromFile("groundMud_base_color.gif"));
    textureMap[BLOCK_EARTH] = std::make_shared<Texture>(Texture::loadFromFile("groundEarth_base_color.gif"));
    textureMap[BLOCK_SNOW] = std::make_shared<Texture>(Texture::loadFromFile("snow_base_color.gif"));
    textureMap[BLOCK_ROCK] = std::make_shared<Texture>(Texture::loadFromFile("stone04_base_color.gif"));
    textureMap[BLOCK_WATER] = std::make_shared<Texture>(Texture::loadFromFile("water_base_color.gif"));

    Shader fragmentShader = Shader::loadFromFile("mesh.frag", Shader::Type::Fragment);
    Shader vertexShader = Shader::loadFromFile("mesh.vert", Shader::Type::Vertex);
    this->shaderProgram = std::make_shared<ShaderProgram>();
    this->shaderProgram->attachShader(vertexShader);
    this->shaderProgram->attachShader(fragmentShader);
    this->shaderProgram->setAttribLocation("vertex_position", 0);
    this->shaderProgram->setAttribLocation("texture_coordinate", 1);
    this->shaderProgram->link();

    for (int x = 0; x < WORLD_X; x++) {
        for (int y = 0; y < WORLD_Y; y++) {
            for (int z = 0; z < WORLD_Z; z++) {
                if (world[x][y][z] == 0) {
                    continue;
                }
                if (!isVisible(world, x, y, z)) {
                    continue;
                }

                glm::mat4 cubeModel = glm::mat4(1.0f);
                glm::vec3 cubePosition = glm::vec3(x, y, z);
                cubeModel = glm::translate(cubeModel, cubePosition);

                blocks[world[x][y][z]].push_back({cubeModel, glm::ivec3(x, y, z)});
            }
        }
    }

    int renderedBlocks = 0;
    for (auto blockType : blocks) {
        renderedBlocks += blockType.second.size();
    }
    std::cout << "Rendered blocks:" << renderedBlocks << std::endl;
}

void VoxelWorld::render(glm::mat4 vp, glm::vec3 cameraPos, bool wireframe) {
    this->shaderProgram->use();
    for (auto blockType : blocks) {
        textureMap[blockType.first]->bind();
        for (auto block : blockType.second) {
            if (glm::distance(block.position, cameraPos) > 50.0) {
                continue;
            }

            auto mvp = vp * block.model;
            this->shaderProgram->setUniform("mvp", mvp);
            this->block->draw(wireframe);
        }
    }
}
