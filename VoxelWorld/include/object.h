#ifndef OBJECT_H
#define OBJECT_H

#include <GL/glew.h>

#include <vector>

#include <glm/vec3.hpp>

class Object {
public:
    Object(std::vector<glm::vec3> vertics, std::vector<glm::uvec3> indices);
    Object(std::vector<glm::vec3> vertics, std::vector<glm::uvec3> indices, std::vector<glm::vec3> colors);
    void draw(bool wireframe);

private:
    GLuint vertexAttributeObject = 0;
    unsigned int incidesCount = 0;
};

#endif
