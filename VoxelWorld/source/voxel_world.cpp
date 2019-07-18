#include "voxel_world.h"

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

const char M = 1; // mud
const char E = 2; // earth with gras on top
const char S = 3; // snow
const char R = 4; // rock
const char W = 5; // water

const int WORLD_X = 100;
const int WORLD_Y = 30;
const int WORLD_Z = 100;

static bool needsRender(char world[WORLD_X][WORLD_Y][WORLD_Z], int x, int y, int z) {
    if (x < 0 || y < 0 || z < 0 || x >= WORLD_X || y >= WORLD_Y || z >= WORLD_Z) {
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

    char world[WORLD_X][WORLD_Y][WORLD_Z] = {{{0}}};

    for (int x = 0; x < WORLD_X; x++) {
        for (int z = 0; z < WORLD_Z; z++) {
            const double value = noise.noise0_1(double(x) / double(WORLD_X), double(z) / double(WORLD_Y));
            const int height = value * WORLD_Y;
            for (int y = 0; y < height; y++) {
                world[x][y][z] = 1;
            }
        }
    }

    block = std::make_shared<Mesh>(Mesh::loadFromFile("block.obj"));

    textureMap[M] = std::make_shared<Texture>(Texture::loadFromFile("groundMud_base_color.gif"));
    textureMap[E] = std::make_shared<Texture>(Texture::loadFromFile("groundEarth_base_color.gif"));
    textureMap[S] = std::make_shared<Texture>(Texture::loadFromFile("snow_base_color.gif"));
    textureMap[R] = std::make_shared<Texture>(Texture::loadFromFile("stone04_base_color.gif"));
    textureMap[W] = std::make_shared<Texture>(Texture::loadFromFile("water_base_color.gif"));

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
                blocks.push_back({world[x][y][z], cubeModel});
            }
        }
    }
}

void VoxelWorld::render(glm::mat4 vp, bool wireframe) {
    this->shaderProgram->use();
    for (auto block : blocks) {
        auto mvp = vp * block.model;
        this->shaderProgram->setUniform("mvp", mvp);
        textureMap[block.textureId]->bind();
        this->block->draw(wireframe);
    }
}
