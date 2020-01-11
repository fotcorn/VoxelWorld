// source:
// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection

#ifndef RAY_H
#define RAY_H

#include <glm/vec3.hpp>

class Ray {
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin), direction(direction) {
        inverseDirection = glm::vec3(1.0f) / direction;
        sign[0] = (inverseDirection.x < 0.0f);
        sign[1] = (inverseDirection.y < 0.0f);
        sign[2] = (inverseDirection.z < 0.0f);
    }
    glm::vec3 origin, direction;
    glm::vec3 inverseDirection;
    int sign[3];
};

#endif
