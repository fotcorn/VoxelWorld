#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include "Shader.h"

#include <string>

#include <GL/glew.h>

#include <glm/mat4x4.hpp>

class ShaderProgram {
public:
    ShaderProgram();
    void attachShader(Shader shader);
    void setAttribLocation(const std::string& attribute, unsigned int location);
    void setUniform(const std::string& uniform, glm::mat4 data);
    void setUniform(const std::string& uniform, int data);
    void setUniform(const std::string& uniform, glm::vec3 data);
    void link();
    void use();

private:
    GLuint handle;
};

#endif
