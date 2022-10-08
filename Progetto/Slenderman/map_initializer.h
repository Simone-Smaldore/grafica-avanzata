#pragma once

#include <map>
#include <time.h>
#include <stdexcept>
#include <vector>

#include "glm/glm.hpp"

#include "collision_solver.h"
#include "constants.h"
#include "model_cache.h"
#include "page.h"
#include "renderable.h"
#include "renderable_poi.h"
#include "street_light.h"

class MapInitializer {
private:
    static bool _isGoodPOI(const int k, const std::map<int, glm::vec3>& poi, const int kMax, const int numVAOForSide);

    static glm::mat4 _computePOITransformForModel(EModel model, const glm::vec3& poiTranslation);

public:
    static std::map<int, glm::vec3> initPOI();

    static void addPOIRenderablesAndStreetLights(const std::map<int, glm::vec3>& poiInfo, std::vector<Page*>& pages, vector<Renderable*>& renderables, CollisionSolver& collisionSolver);
};

bool MapInitializer::_isGoodPOI(const int k, const std::map<int, glm::vec3>& poi, const int kMax, const int numVAOForSide) {
    // Verifica che il punto non è già stato escluso a priori nella configurazione
    if (K_SET_TO_EXCLUDE.find(k) != K_SET_TO_EXCLUDE.end())
        return false;

    // Esclude i 4 lati esterni
    if (k < numVAOForSide || k >(kMax - numVAOForSide))
        return false;
    if (k % numVAOForSide == 0 || (k + 1) % numVAOForSide == 0)
        return false;

    // Verifica che il punto non sia adiacente ad un altro punto già estratto
    for (auto poiInfo : poi) {
        int val = poiInfo.first;
        if (k + 1 == val || k - 1 == val)
            return false;

        if (k + numVAOForSide == val || k + numVAOForSide + 1 == val || k + numVAOForSide - 1 == val)
            return false;

        if (k - numVAOForSide == val || k - numVAOForSide + 1 == val || k - numVAOForSide - 1 == val)
            return false;

    }
    return true;
}

std::map<int, glm::vec3> MapInitializer::initPOI() {
    std::map<int, glm::vec3> poiMap;

    srand(time(NULL));

    int numVAOForSide = TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_TREE;
    int kMax = (numVAOForSide * numVAOForSide) - 1;

    for (int i = 0; i < NUMBER_POINTS_OF_INTEREST; i++) {
        int k = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * kMax;
        while (!_isGoodPOI(k, poiMap, kMax, numVAOForSide)) {
            k = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * kMax;
        }

        int x_index = k / numVAOForSide;
        int z_index = k % numVAOForSide;
        int offset_x_index = x_index - numVAOForSide / 2;
        int offset_z_index = z_index - numVAOForSide / 2;
        float x_offset = (TREE_OFFSET * VAO_OBJECTS_SIDE_TREE * offset_x_index) + TREE_OFFSET;
        float z_offset = (TREE_OFFSET * VAO_OBJECTS_SIDE_TREE * offset_z_index) + TREE_OFFSET;

        poiMap.insert({ k, glm::vec3(x_offset, -4.0f, z_offset) });
    }

    return poiMap;
}

glm::mat4 MapInitializer::_computePOITransformForModel(EModel model, const glm::vec3& poiTranslation) {
    glm::mat4 transform = glm::mat4(1.0f);

    switch (model) {
    case EModel::poi1:
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.008f, 0.008f, 0.008f));
        break;

    case EModel::poi2:
        transform = glm::translate(transform, glm::vec3(0.0f, -0.1f, 0.0f));
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.03f, 0.03f, 0.03f));
        transform = glm::rotate(transform, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
        break;

    case EModel::poi3:
        transform = glm::translate(transform, glm::vec3(0.0f, -0.1f, 0.0f));
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.05f, 0.05f, 0.05f));
        transform = glm::rotate(transform, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
        break;

    case EModel::poi4:
        transform = glm::translate(transform, glm::vec3(0.0f, -3.1f, 0.0f));
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(8.5f, 8.5f, 8.5f));
        break;

    case EModel::poi5:
        transform = glm::translate(transform, glm::vec3(0.0f, 1.4f, 0.0f));
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.05f, 0.05f, 0.05f));
        break;

    case EModel::poi6:
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.1f, 0.1f, 0.1f));
        break;

    case EModel::poi7:
        transform = glm::translate(transform, glm::vec3(0.0f, -0.8f, 0.0f));
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.1f, 0.1f, 0.1f));
        transform = glm::rotate(transform, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
        break;

    case EModel::poi8:
        transform = glm::translate(transform, glm::vec3(-60.0f, 3.8f, 60.0f));
        transform = glm::translate(transform, poiTranslation);
        transform = glm::scale(transform, glm::vec3(0.008f, 0.008f, 0.008f));
        transform = glm::rotate(transform, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
        break;

    default:
        throw std::runtime_error("Unknown transform for poi index: " + static_cast<int>(model));
    }

    return transform;
}

void MapInitializer::addPOIRenderablesAndStreetLights(const std::map<int, glm::vec3>& poiInfo, std::vector<Page*>& pages, vector<Renderable*>& renderables, CollisionSolver& collisionSolver) {
    int i = 0;
    
    for (auto poi : poiInfo) {
        ETexture texture = static_cast<ETexture>(i + static_cast<int>(ETexture::poi1));
        EModel model = static_cast<EModel>(i + static_cast<int>(EModel::poi1));

        glm::mat4 poiTransform = _computePOITransformForModel(model, poi.second);
        RenderablePOI* renderablePOI = new RenderablePOI(texture, model, poiTransform);
        renderables.push_back(renderablePOI);
        collisionSolver.registerAABB(renderablePOI->toAABB());

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
        transform = glm::translate(transform, poi.second);
        transform = glm::scale(transform, glm::vec3(0.015f, 0.015f, 0.015f));
        StreetLight* streetLight = new StreetLight(transform);
        renderables.push_back(streetLight);
        collisionSolver.registerAABB(streetLight->toAABB());

        ETexture pageTexture = static_cast<ETexture>(i + static_cast<int>(ETexture::page1));
        Page* page = new Page(pageTexture, poi.second);
        pages.push_back(page);
        renderables.push_back(page);

        i += 1;
    }
}