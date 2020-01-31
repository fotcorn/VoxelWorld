#ifndef IMAGE_H
#define IMAGE_H

#include "ShaderProgram.h"
#include "Sprite.h"
#include "Texture.h"

#include <memory>

class Image : public Sprite {

public:
    Image(float x, float y, float z, float width, float height, std::shared_ptr<Texture> texture);
    void render(glm::mat4 projectionMatrix2D, bool wireframe) override;

private:
    float x, y, z, width, height;
    std::shared_ptr<Texture> texture;

    static void init();
    static std::shared_ptr<ShaderProgram> shaderProgram;
    static GLuint vao;
    static GLuint vbo;
};

#endif /* IMAGE_H */
