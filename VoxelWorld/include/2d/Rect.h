#ifndef RECT_H
#define RECT_H

#include "ShaderProgram.h"

#include <memory>

class Rect {

public:
    Rect(float x, float y, float z, float width, float height, glm::vec3 color);
    void render(glm::mat4 projectionMatrix2D, bool wireframe);

private:
    float x, y, z, width, height;
    glm::vec3 color;

    static void init();
    static std::shared_ptr<ShaderProgram> shaderProgram;
    static GLuint vao;
    static GLuint vbo;
};

#endif /* RECT_H */
