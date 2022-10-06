#pragma once

#include <map>
#include <time.h>

#include "glm/glm.hpp"

#include "constants.h"

class MapInitializer {
private:
    static bool isGoodPOI(const int k, const std::map<int, glm::vec3>& poi, const int kMax, const int numVAOForSide);

public:
    static std::map<int, glm::vec3> initPOI();
};

bool MapInitializer::isGoodPOI(const int k, const std::map<int, glm::vec3>& poi, const int kMax, const int numVAOForSide) {
    // Verifica che il punto non è già stato escluso a priori nella configurazione
    if (std::find(K_MAP_TO_EXCLUDE.begin(), K_MAP_TO_EXCLUDE.end(), k) != K_MAP_TO_EXCLUDE.end())
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
        while (!isGoodPOI(k, poiMap, kMax, numVAOForSide)) {
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