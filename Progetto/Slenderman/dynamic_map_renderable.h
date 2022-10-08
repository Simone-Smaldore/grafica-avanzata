#pragma once

#include <stdexcept>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "constants.h"
#include "light_utils.h"
#include "model_cache.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

enum class DynamicEntity {
    tree,
    grass
};

class DynamicMapRenderable : public InstancedModelRenderable {
private:
    const DynamicEntity _entity;
    const unordered_set<int> _tabooIndices;

private:
    vector<int> getVaoIndexesFromCamera(const Camera& camera, const float offset, const int quadSide, const int vaoObjectSide) const;
    void renderDynamicMap(const vector<int>& VAOIndexes, const int quadSide, const int vaoObjectSide) const;

public:
    DynamicMapRenderable(const DynamicEntity entity, const unordered_set<int> tabooIndices = { });

    inline std::vector<aabb> toAABBs() const;

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

DynamicMapRenderable::DynamicMapRenderable(const DynamicEntity entity, const unordered_set<int> tabooIndices) : _entity(entity), _tabooIndices(tabooIndices) {
    switch (_entity) {
    case DynamicEntity::tree:
        _model = ModelCache::getInstance().findModel(EModel::tree);
        _shader = ShaderCache::getInstance().findShader(EShader::tree);
        _initUsingDynamicMapAlgorithm(TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE, TREE_OFFSET, glm::vec3(0.08f, 0.08f, 0.08f), false, tabooIndices);
        break;
    case DynamicEntity::grass:
        _model = ModelCache::getInstance().findModel(EModel::grass);
        _shader = ShaderCache::getInstance().findShader(EShader::grass);
        _initUsingDynamicMapAlgorithm(GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS, GRASS_OFFSET, glm::vec3(0.015f, 0.01f, 0.015f), true);
        break;
    }
}

vector<int> DynamicMapRenderable::getVaoIndexesFromCamera(const Camera& camera, const float offset, const int quadSide, const int vaoObjectSide) const {
    vector<int> result;

    float xCamera = camera.Position.x + (offset * quadSide / 2) + offset / 2;
    if (xCamera < 0.0f) {
        xCamera = 0.0f;
    }
    if (xCamera >= offset * quadSide) {
        xCamera = offset * quadSide - 0.1f;
    }

    float zCamera = camera.Position.z + (offset * quadSide / 2) + offset / 2;
    if (zCamera < 0.0f) {
        zCamera = 0.0f;
    }
    if (zCamera >= offset * quadSide) {
        zCamera = offset * quadSide - 0.1f;
    }

    int xIndex = floor(xCamera / (vaoObjectSide * offset));
    int zIndex = floor(zCamera / (vaoObjectSide * offset));

    for (int i = (xIndex - INT_OFFSET_VAO_INDEXES); i <= (xIndex + INT_OFFSET_VAO_INDEXES); i++) {
        for (int j = (zIndex - INT_OFFSET_VAO_INDEXES); j <= (zIndex + INT_OFFSET_VAO_INDEXES); j++) {
            int vaoIndex = (i * (quadSide / vaoObjectSide)) + j;
            result.push_back(vaoIndex);
        }
    }

    return result;
}

void DynamicMapRenderable::renderDynamicMap(const vector<int>& VAOIndexes, const int quadSide, const int vaoObjectSide) const {
    unsigned int numVAO = (quadSide / vaoObjectSide) * (quadSide / vaoObjectSide);
    unsigned int numElementForVAO = (quadSide * quadSide) / numVAO;

    for (unsigned int k = 0; k < VAOIndexes.size(); k++) {
        int vaoIndex = std::max(VAOIndexes[k], 0);
        if (_tabooIndices.find(vaoIndex) != _tabooIndices.end())
            continue;
        for (unsigned int i = 0; i < _model->meshes.size(); i++) {
            for (unsigned int j = 0; j < _model->meshes[i].textures.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                string name = _model->meshes[i].textures[j].type;
                glUniform1i(glGetUniformLocation(_shader->ID, (name + std::to_string(j + 1)).c_str()), j);
                glBindTexture(GL_TEXTURE_2D, _model->meshes[i].textures[j].id);
            }
            glBindVertexArray(_model->meshes[i].VAOs[vaoIndex]);
            glDrawElementsInstanced(GL_TRIANGLES, _model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0, numElementForVAO);
            glBindVertexArray(0);
        }
    }
}

std::vector<aabb> DynamicMapRenderable::toAABBs() const {
    if (_entity == DynamicEntity::grass)
        throw std::runtime_error("Cannot compute grass AABBs");

    std::vector<aabb> result;
    for (auto transform : _transforms) {
        // nTODO: Eliminare centroidi magici?
        auto aabbs = aabb::fromCompoundModel(*(_model), { glm::vec3(18.0f, 0.0f, -31.0f), glm::vec3(-246.0f, 0.0f, 280.0f), glm::vec3(59.0f, 0.0f, 311.0f) }, transform);
        result.insert(result.end(), aabbs.begin(), aabbs.end());
    }

    return result;
}

void DynamicMapRenderable::render(const Camera& camera, const LightUtils& lightUtils) {
    lightUtils.initLightShader(_shader, camera);

    _shader->use();
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setFloat("alphaValue", 0.4f);

    vector<int> VAOIndexes;
    switch (_entity) {
    case DynamicEntity::tree:
        VAOIndexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE);
        renderDynamicMap(VAOIndexes, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE);
        break;
    case DynamicEntity::grass:
        VAOIndexes = getVaoIndexesFromCamera(camera, GRASS_OFFSET, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);
        renderDynamicMap(VAOIndexes, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);
        break;
    }
}