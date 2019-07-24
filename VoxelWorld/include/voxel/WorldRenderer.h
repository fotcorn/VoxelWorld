#ifndef WORLD_RENDERER_H
#define WORLD_RENDERER_H

#include "ShaderProgram.h"
#include "Texture.h"
#include "WorldGenerator.h"

#include <memory>

class WorldRenderer {
public:
    void init();
    void render(glm::mat4 vp, glm::vec3 cameraPos, bool wireframe);

private:
    Texture texture;
    ShaderProgram shaderProgram;
    WorldGenerator worldGenerator;
};

#endif // !WORLD_RENDERER_H
