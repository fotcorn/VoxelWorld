#ifndef VOXEL_WORLD_H
#define VOXEL_WORLD_H

#include <glm/mat4x4.hpp>

#include <memory>
#include <unordered_map>

#include "mesh.h"
#include "shader_program.h"
#include "texture.h"

class VoxelWorld {
    std::unordered_map<char, std::shared_ptr<Texture>> textureMap;
    std::unordered_map<char, std::vector<glm::mat4>> blocks;
    std::shared_ptr<Mesh> block;
    std::shared_ptr<ShaderProgram> shaderProgram;

public:
    void init();

    void render(glm::mat4 vp, bool wireframe);
};

#endif // !VOXEL_WORLD_H
