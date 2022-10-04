#pragma once

#include <vector>

#include "model.h"

class SimpleVertexClusterer {
private:
    const vector<glm::vec3>& centroids;

public:
    SimpleVertexClusterer(const vector<glm::vec3>& _centroids) : centroids(_centroids) {}

    vector<vector<Vertex>> generateVertexClusters(const Model& model, const float yMax = FLT_MAX) const;
};

vector<vector<Vertex>> SimpleVertexClusterer::generateVertexClusters(const Model& model, const float yMax) const {
    vector<vector<Vertex>> result;

    for (int i = 0; i < centroids.size(); i++)
        result.push_back(vector<Vertex>());

    for (const auto& mesh : model.meshes) {
        for (const auto& vertex : mesh.vertices) {
            if (vertex.Position.y > yMax)
                continue;

            int minIndex = 0;
            float minDistance = glm::distance(vertex.Position, centroids[0]);
            for (int i = 1; i < centroids.size(); i++) {
                float currentDistance = glm::distance(vertex.Position, centroids[i]);
                if (currentDistance < minDistance) {
                    minIndex = i;
                    minDistance = currentDistance;
                }
            }

            result[minIndex].push_back(vertex);
        }
    }

    return result;
}