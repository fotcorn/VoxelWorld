#ifndef RECT_H
#define RECT_H

#include "2d/Sprite.h"
#include "ShaderProgram.h"

#include <memory>

/**
 * A rectangular 2D shape with a color.
 */
class Rect : public Sprite {

public:
    Rect(float x, float y, float z, float width, float height, glm::vec3 color);
    void render(glm::mat4 projectionMatrix2D, bool wireframe) override;

private:
    float x, y, z, width, height;
    glm::vec3 color;

    static void init();
    static std::shared_ptr<ShaderProgram> shaderProgram;
    static GLuint vao;
    static GLuint vbo;
};

#endif /* RECT_H */
