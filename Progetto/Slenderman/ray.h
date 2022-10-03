#pragma once

#include "glm/glm.hpp"

struct ray {
    glm::vec3 origin;
    glm::vec3 direction;

    ray(glm::vec3 p_origin, glm::vec3 p_direction) : origin(p_origin), direction(p_direction) {}

    inline ray reflect(const glm::vec3& axis) const;

    ray rotate(const float angle, const glm::vec3& axis) const;
};

inline ray ray::reflect(const glm::vec3& axis) const {
    return ray(origin, glm::reflect(direction, axis));
}

ray ray::rotate(const float angle, const glm::vec3& axis) const {
    glm::mat4 rotationMatrix(1);
    rotationMatrix = glm::rotate(rotationMatrix, angle, axis);
    return ray(origin, glm::vec3(rotationMatrix * glm::vec4(direction, 1.0)));
}