#ifndef SPRITE_H
#define SPRITE_H

#include <glm/mat4x4.hpp>

class Sprite {
public:
    virtual void render(glm::mat4 projectionMatrix2D, bool wireframe) = 0;
};

#endif /* SPRITE_H */
