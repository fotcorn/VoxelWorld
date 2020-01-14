#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include "RenderChunkGenerator.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "WorldGenerator.h"

#include <memory>

class WorldRenderer {
public:
    void init(int cameraChunkDistance);
    void render(World& world, glm::mat4 vp, glm::vec3 cameraPos, glm::vec3 cameraFront, bool wireframe);

private:
    int cameraChunkDistance;
    std::shared_ptr<Texture> texture;
    ShaderProgram shaderProgram;
    std::shared_ptr<RenderChunkGenerator> renderChunkGenerator;
};

#endif // !WORLD_RENDERER_H
