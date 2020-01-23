#ifndef RAY_H
#define RAY_H

#include <optional>

#include <glm/vec3.hpp>

class Ray {
public:
    Ray(const glm::vec3& origin, const glm::vec3& direction) : origin(origin) {
        inverseDirection = glm::vec3(1.0f) / direction;
    }

    // source: https://gamedev.stackexchange.com/a/18459
    std::optional<float> AABBintersect(const glm::vec3& lb, const glm::vec3& rt) {
        // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
        float t1 = (lb.x - origin.x) * inverseDirection.x;
        float t2 = (rt.x - origin.x) * inverseDirection.x;
        float t3 = (lb.y - origin.y) * inverseDirection.y;
        float t4 = (rt.y - origin.y) * inverseDirection.y;
        float t5 = (lb.z - origin.z) * inverseDirection.z;
        float t6 = (rt.z - origin.z) * inverseDirection.z;

        float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
        float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

        float t = 0.0f;

        // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
        if (tmax < 0) {
            t = tmax;
            return std::nullopt;
        }

        // if tmin > tmax, ray doesn't intersect AABB
        if (tmin > tmax) {
            t = tmax;
            return std::nullopt;
        }

        t = tmin;
        return std::optional<float>(t);
    }

private:
    glm::vec3 origin;
    glm::vec3 inverseDirection;
};

#endif
