#pragma once

#include "glm/glm.hpp"

#include "collision_solver.h"
#include "model.h"
#include "ray.h"
#include "vertex_clusterer.h"

class aabb {
private:
    friend class CollisionSolver;

    glm::vec3 min;
    glm::vec3 max;

    unsigned int _vao = 0;

    bool _hasIntersection = false;
    bool _shouldBeRendered = false;

    static void _updateWithVertex(const Vertex& vertex, glm::vec3& min, glm::vec3& max);

    static void _updateWithVertex(const glm::vec3& vertex, glm::vec3& min, glm::vec3& max);

    static aabb* _applyTransformToMinMax(const glm::mat4& transform, const glm::vec3& currentMin, const glm::vec3& currentMax);

    void _resetCurrentIntersection();

public:

    aabb(glm::vec3 p_min, glm::vec3 p_max) : min(p_min), max(p_max) {}

    static aabb* fromModel(const Model& model, const glm::mat4& transform = glm::mat4(1));

    static vector<aabb*> fromCompoundModel(const Model& model, const vector<glm::vec3>& centroids, const glm::mat4& transform = glm::mat4(1));

    bool intersectRay2D(const ray& ray, const float& maxDistance = 5.0f);

    inline bool hasIntersection() const { return _hasIntersection; }

    inline glm::vec3 getMin() const { return min; }

    inline glm::vec3 getMax() const { return max; }

    inline bool shouldBeRendered() { return _shouldBeRendered; }

    inline void disableRendering() { _shouldBeRendered = false; }
};

void aabb::_resetCurrentIntersection() {
    _hasIntersection = false;
    _shouldBeRendered = true;
}

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

aabb* aabb::_applyTransformToMinMax(const glm::mat4& transform, const glm::vec3& currentMin, const glm::vec3& currentMax) {
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

    return new aabb(min, max);
}

aabb* aabb::fromModel(const Model& model, const glm::mat4& transform) {
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

    return new aabb(min, max);
}

vector<aabb*> aabb::fromCompoundModel(const Model& model, const vector<glm::vec3>& centroids, const glm::mat4& transform) {
    vector<aabb*> result;

    SimpleVertexClusterer simpleVertexClusterer(centroids);
    auto clusters = simpleVertexClusterer.generateVertexClusters(model, 8.0f, 24.0f);

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

bool aabb::intersectRay2D(const ray& ray, const float& maxDistance) {
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
