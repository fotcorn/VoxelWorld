#ifndef RECT_H
#define RECT_H

#include "ShaderProgram.h"

#include <memory>

class Rect {

public:
    Rect(int x, int y, int width, int height);
    void render(glm::mat4 projectionMatrix2D, bool wireframe);

private:
    int x, y, width, height;

    static void init();
    static std::shared_ptr<ShaderProgram> shaderProgram;
    static GLuint vao;
    static GLuint vbo;
};

#endif /* RECT_H */
