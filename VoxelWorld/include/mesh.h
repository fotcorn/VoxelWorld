#ifndef MODEL_H
#define MODEL_H

#include "texture.h"
#include "vertex.h"

#include <string>
#include <vector>

#include <GL/glew.h>

class Mesh {
public:
    static Mesh loadFromFile(const std::string& path);
    void addTexture(Texture texture);
    void draw(bool wireframe);

private:
    Mesh() = default;
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    GLuint vao;
    GLuint vbo;
    GLuint ebo;
};

#endif // !MODEL_H
