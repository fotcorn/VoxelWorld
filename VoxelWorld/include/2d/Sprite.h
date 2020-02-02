#ifndef SPRITE_H
#define SPRITE_H

#include <glm/mat4x4.hpp>

/**
 * Base class for 2D shapes
 */
class Sprite {
public:
    virtual void render(glm::mat4 projectionMatrix2D, bool wireframe) = 0;
    virtual ~Sprite() = default;
};

#endif /* SPRITE_H */
