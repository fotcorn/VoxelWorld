#ifndef RECT_H
#define RECT_H

#include "ShaderProgram.h"

class Rect {

public:
    Rect(int x, int y, int width, int height);
    virtual ~Rect();
    void render(glm::mat4 projectionMatrix2D, bool wireframe);

private:
    int x, y, width, height;
    ShaderProgram shaderProgram;
    GLuint vao;
    GLuint vbo;
};

#endif /* RECT_H */
