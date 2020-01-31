#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

#include <memory>
#include <string>
#include <unordered_map>

class Texture {
public:
    static std::shared_ptr<Texture> loadFromFile(const std::string& path);

    void bind();

private:
    Texture() = default;
    GLuint handle = 0;

    static std::unordered_map<std::string, std::shared_ptr<Texture>> textureCache;
};

#endif // !TEXTURE_H
