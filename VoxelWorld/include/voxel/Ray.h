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

    bool AABBintersect(const glm::vec3 bounds[2]) {
        float tmin = (bounds[sign[0]].x - origin.x) * inverseDirection.x;
        float tmax = (bounds[1 - sign[0]].x - origin.x) * inverseDirection.x;
        float tymin = (bounds[sign[1]].y - origin.y) * inverseDirection.y;
        float tymax = (bounds[1 - sign[1]].y - origin.y) * inverseDirection.y;

        if ((tmin > tymax) || (tymin > tmax)) {
            return false;
        }
        if (tymin > tmin) {
            tmin = tymin;
        }
        if (tymax < tmax) {
            tmax = tymax;
        }

        float tzmin = (bounds[sign[2]].z - origin.z) * inverseDirection.z;
        float tzmax = (bounds[1 - sign[2]].z - origin.z) * inverseDirection.z;

        if ((tmin > tzmax) || (tzmin > tmax)) {
            return false;
        }
        return true;
    }

private:
    glm::vec3 origin, direction;
    glm::vec3 inverseDirection;
    int sign[3];
};

#endif
