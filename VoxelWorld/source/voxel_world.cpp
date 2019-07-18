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
#include "shader.h"
#include "shader_program.h"
#include "texture.h"

const char M = 1; // mud
const char E = 2; // earth with gras on top
const char S = 3; // snow
const char R = 4; // rock
const char W = 5; // water

const int CHUNK_SIZE = 4;
const char chunk[CHUNK_SIZE][CHUNK_SIZE][CHUNK_SIZE] = {{
                                                            {M, M, M, M},
                                                            {M, M, M, M},
                                                            {M, M, M, M},
                                                            {M, M, M, M},
                                                        },
                                                        {
                                                            {M, M, M, M},
                                                            {M, M, M, M},
                                                            {M, M, W, M},
                                                            {M, M, M, M},
                                                        },
                                                        {
                                                            {M, M, M, M},
                                                            {M, M, M, M},
                                                            {M, M, 0, 0},
                                                            {M, M, 0, 0},
                                                        },
                                                        {
                                                            {E, E, E, E},
                                                            {E, E, 0, E},
                                                            {E, 0, 0, 0},
                                                            {E, 0, 0, 0},
                                                        }};

void VoxelWorld::init() {
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

    for (int x = 0; x < CHUNK_SIZE; x++) {
        for (int y = 0; y < CHUNK_SIZE; y++) {
            for (int z = 0; z < CHUNK_SIZE; z++) {
                if (chunk[x][y][z] == 0) {
                    continue;
                }

                glm::mat4 cubeModel = glm::mat4(1.0f);
                glm::vec3 cubePosition = glm::vec3(y, x, z);
                cubeModel = glm::translate(cubeModel, cubePosition);
                blocks.push_back({chunk[x][y][z], cubeModel});
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
