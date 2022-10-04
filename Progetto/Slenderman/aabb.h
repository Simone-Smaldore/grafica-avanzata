#pragma once

#include "glm/glm.hpp"

#include "model.h"
#include "ray.h"
#include "vertex_clusterer.h"

class aabb {
private:
    friend class CollisionSolver;

    glm::vec3 min;
    glm::vec3 max;

    unsigned int _vao = 0;
    mutable bool _hasIntersection = false;

    static void _updateWithVertex(const Vertex& vertex, glm::vec3& min, glm::vec3& max);

    static void _updateWithVertex(const glm::vec3& vertex, glm::vec3& min, glm::vec3& max);

    static aabb _applyTransformToMinMax(const glm::mat4& transform, const glm::vec3& currentMin, const glm::vec3& currentMax);

    inline void _resetCurrentIntersection() const { _hasIntersection = false; }

public:

    aabb(glm::vec3 p_min, glm::vec3 p_max) : min(p_min), max(p_max) {}

    static aabb fromModel(const Model& model, const glm::mat4& transform = glm::mat4(1));

    static vector<aabb> fromCompoundModel(const Model& model, const vector<glm::vec3>& centroids, const glm::mat4& transform = glm::mat4(1));

    bool intersect(const aabb& other) const;

    bool intersectRay2D(const ray& ray, const float& maxDistance = 5.0f) const;

    unsigned int bindToVAO();

    inline unsigned int vao() const { return _vao; }

    inline bool hasIntersection() const { return _hasIntersection; }
};

void aabb::_updateWithVertex(const Vertex& vertex, glm::vec3& min, glm::vec3& max) {
    _updateWithVertex(vertex.Position, min, max);
}

void aabb::_updateWithVertex(const glm::vec3& vertex, glm::vec3& min, glm::vec3& max) {
    if (vertex.x < min.x)
        min.x = vertex.x;
    else if (vertex.x > max.x)
        max.x = vertex.x;

    if (vertex.y < min.y)
        min.y = vertex.y;
    else if (vertex.y > max.y)
        max.y = vertex.y;

    if (vertex.z < min.z)
        min.z = vertex.z;
    else if (vertex.z > max.z)
        max.z = vertex.z;
}

aabb aabb::_applyTransformToMinMax(const glm::mat4& transform, const glm::vec3& currentMin, const glm::vec3& currentMax) {
    glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    const int kVertices = 8;
    glm::vec3 transformedVertices[kVertices] = {
        glm::vec3(currentMin.x, currentMin.y, currentMin.z),
        glm::vec3(currentMax.x, currentMin.y, currentMin.z),
        glm::vec3(currentMax.x, currentMax.y, currentMin.z),
        glm::vec3(currentMin.x, currentMax.y, currentMin.z),
        glm::vec3(currentMin.x, currentMin.y, currentMax.z),
        glm::vec3(currentMax.x, currentMin.y, currentMax.z),
        glm::vec3(currentMax.x, currentMax.y, currentMax.z),
        glm::vec3(currentMin.x, currentMax.y, currentMax.z),
    };

    for (int i = 0; i < kVertices; i++)
        _updateWithVertex(glm::vec3(transform * glm::vec4(transformedVertices[i], 1.0f)), min, max);

    return aabb(min, max);
}

aabb aabb::fromModel(const Model& model, const glm::mat4& transform) {
    glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
    glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

    for (const auto& mesh : model.meshes) {
        for (const auto& vertex : mesh.vertices) {
            // TODO: troncare le y in base alla dimesione del modello e al relativo transform (rotazione e scale)
            // Ok per il lampione, ma non per il POI ad esempio
            //if (position.y > 500.0f)
                //continue;
            _updateWithVertex(vertex, min, max);
        }
    }

    // TODO: il transform andrebbe computato una volta in via preventiva (per tutti i modelli)
    // Al momento viene ricalcolato volta dopo volta nel renderer
    min = glm::vec3(transform * glm::vec4(min, 1.0f));
    max = glm::vec3(transform * glm::vec4(max, 1.0f));

    return aabb(min, max);
}

vector<aabb> aabb::fromCompoundModel(const Model& model, const vector<glm::vec3>& centroids, const glm::mat4& transform) {
    vector<aabb> result;

    SimpleVertexClusterer simpleVertexClusterer(centroids);
    auto clusters = simpleVertexClusterer.generateVertexClusters(model, 75.0f);

    for (const auto& cluster : clusters) {
        glm::vec3 min(FLT_MAX, FLT_MAX, FLT_MAX);
        glm::vec3 max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        for (const auto& vertex : cluster) {
            _updateWithVertex(vertex, min, max);
        }

        result.push_back(_applyTransformToMinMax(transform, min, max));
    }

    return result;
}

bool aabb::intersect(const aabb& other) const {
    _hasIntersection = min.x <= other.max.x &&
        max.x >= other.min.x &&
        min.y <= other.max.y &&
        max.y >= other.min.y &&
        // L'asse z � invertita
        min.z >= other.max.z &&
        max.z <= other.min.z;
    return _hasIntersection;
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

    _hasIntersection = true;
    return true;
}

unsigned int aabb::bindToVAO() {
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

    _vao = VAO;
    return VAO;
}