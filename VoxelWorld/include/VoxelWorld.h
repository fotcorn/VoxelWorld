#ifndef VOXEL_WORLD_H
#define VOXEL_WORLD_H

#include <glm/mat4x4.hpp>

#include <memory>
#include <unordered_map>

#include "Mesh.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "TextureAtlas.h"

struct Block {
    glm::mat4 model;
    glm::vec3 position;
    TextureAtlas type;
};

class VoxelWorld {
    std::vector<Block> blocks;
    std::shared_ptr<Mesh> block;
    std::shared_ptr<Texture> texture;
    std::shared_ptr<ShaderProgram> shaderProgram;

public:
    void init();

    void render(glm::mat4 vp, glm::vec3 cameraPos, bool wireframe);
};

#endif // !VOXEL_WORLD_H
