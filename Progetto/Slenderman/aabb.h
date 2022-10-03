#pragma once

#include "glm/glm.hpp"

#include "model.h"
#include "ray.h"

class aabb {
private:
    glm::vec3 min;
    glm::vec3 max;

public:
    aabb(glm::vec3 p_min, glm::vec3 p_max) : min(p_min), max(p_max) {}

    static aabb fromModel(const Model& model);

    bool intersect(const aabb& other) const;

    bool intersectRay2D(const ray& ray, const float& maxDistance = 5.0f) const;

    unsigned int bindToVAO() const;
};

aabb aabb::fromModel(const Model& model) {
    glm::vec3 min(0, 0, 0);
    glm::vec3 max(0, 0, 0);

    // TODO: Implementare

    return aabb(min, max);
}

bool aabb::intersect(const aabb& other) const {
    return min.x <= other.max.x &&
        max.x >= other.min.x &&
        min.y <= other.max.y &&
        max.y >= other.min.y &&
        // L'asse z è invertita
        min.z >= other.max.z &&
        max.z <= other.min.z;
}

bool aabb::intersectRay2D(const ray& ray, const float& maxDistance) const {
    float txMin = (min.x - ray.origin.x) / ray.direction.x;
    float txMax = (max.x - ray.origin.x) / ray.direction.x;

    if (txMin > txMax)
        swap(txMin, txMax);

    if (txMax > maxDistance)
        txMax = maxDistance;
    if (txMin < 0)
        txMin = 0;

    float tzMin = (min.z - ray.origin.z) / ray.direction.z;
    float tzMax = (max.z - ray.origin.z) / ray.direction.z;

    if (tzMin > tzMax)
        swap(tzMin, tzMax);

    if (tzMax > maxDistance)
        tzMax = maxDistance;
    if (tzMin < 0)
        tzMin = 0;

    if ((txMin > tzMax) || (tzMin > txMax))
        return false;

    return true;
}

unsigned int aabb::bindToVAO() const {
    float vertices[] = {
        min.x, min.y, min.z,
        max.x, min.y, min.z,
        max.x, max.y, min.z,
        min.x, max.y, min.z,
        min.x, min.y, max.z,
        max.x, min.y, max.z,
        max.x, max.y, max.z,
        min.x, max.y, max.z,
    };

    unsigned int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
    };

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return VAO;
}