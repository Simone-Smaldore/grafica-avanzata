#pragma once
#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/detail/type_vec.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../audio_manager.h"
#include "../camera.h"
#include "../slenderman.h"

class SlenderManager {
public:
    float _previousTime = glfwGetTime();
    glm::vec3 _slendermanTranslationVector = glm::vec3(0.0f, -15.8f, -10000.0f);
    float _fearFactor = 0.0f;
    float _fearUpdateTime = glfwGetTime();

    SlenderManager() {};

    void updateSlenderman(const Camera& camera, SlenderMan& slenderman, const std::vector<glm::vec3>& slendermanSpawnPoints, const int collectedPages);

    float updateFearFactor(const Camera& camera, const float previousFearFactor);

    void resetFearUpdateTime();

private:
    glm::mat4 _getSlendemanShaderModel(const Camera& camera);

    float _calcSlenderRotationAngle(const Camera& camera);

    vector<glm::vec3> _getNearSpawnPoints(const Camera camera, const std::vector<glm::vec3> slendermanSpawnPoints, const int collectedPages);

    float _calcNegativeFearFactor(float distance, float angle, float timeDifference);

    float _calcPositiveFearFactor(float distance, float angle, float timeDifference);
};

void SlenderManager::updateSlenderman(const Camera& camera, SlenderMan& slenderman, const std::vector<glm::vec3>& slendermanSpawnPoints, const int collectedPages) {
    //TODO Eliminare magic numbers (Secondi e distanza)
    if (glfwGetTime() - _previousTime > TIME_SPAWN_SLENDER_FACTOR * (NUM_PAGES - collectedPages) && _fearFactor == 0) {
        vector<glm::vec3> nearSpawnPoints = _getNearSpawnPoints(camera, slendermanSpawnPoints, collectedPages);
        if (nearSpawnPoints.empty()) {
            return;
        }
        int spawnPointIndex = rand() % nearSpawnPoints.size();
        //Scegliere randomicamente e con la direzione dello sguardo
        _slendermanTranslationVector = nearSpawnPoints[spawnPointIndex];
        _previousTime = glfwGetTime();
    }

    glm::mat4 slenderTransform = _getSlendemanShaderModel(camera);
    slenderman.setTransform(slenderTransform);
}

float SlenderManager::updateFearFactor(const Camera& camera, const float previousFearFactor) {
    float slenderDistance = sqrt(pow(_slendermanTranslationVector.x - camera.Position.x, 2) + pow(_slendermanTranslationVector.z - camera.Position.z, 2));
    if (slenderDistance > DISTANCE_RESET_FEAR) {
        // nTODO: Numeri magici...
        if (previousFearFactor > 0.05)
            _fearFactor = previousFearFactor - 0.005;
        else
            _fearFactor = 0;
    }
    else {
        float slenderAngle = atan2((_slendermanTranslationVector.x - camera.Position.x), (_slendermanTranslationVector.z - camera.Position.z)) - atan2(camera.Front.x, camera.Front.z);
        slenderAngle = slenderAngle * 180 / M_PI;
        if (slenderAngle < 0) {
            slenderAngle = slenderAngle + 360.0f;
        }
        float timeDifference = glfwGetTime() - _fearUpdateTime;
        if (slenderAngle >= HALF_SLENDER_CONE_OPENING && slenderAngle <= 360.0f - HALF_SLENDER_CONE_OPENING) {
            _fearFactor = std::max(0.0f, _fearFactor - _calcNegativeFearFactor(slenderDistance, slenderAngle, timeDifference));
        }
        else {
            _fearFactor = std::min(1.0f, _fearFactor + _calcPositiveFearFactor(slenderDistance, slenderAngle, timeDifference));
        }
    }
    AudioManager::getInstance().setMusicVolume(EMusic::whiteNoise, _fearFactor);
    AudioManager::getInstance().setMusicVolume(EMusic::highFear, _fearFactor);
    _fearUpdateTime = glfwGetTime();
    return _fearFactor;
}

void SlenderManager::resetFearUpdateTime() {
    _fearUpdateTime = glfwGetTime();
}

glm::mat4 SlenderManager::_getSlendemanShaderModel(const Camera& camera) {
    glm::mat4 slendermanShaderModel = glm::mat4(1.0f);
    slendermanShaderModel = glm::translate(slendermanShaderModel, _slendermanTranslationVector);
    float rotationAngle = _calcSlenderRotationAngle(camera);
    slendermanShaderModel = glm::rotate(slendermanShaderModel, (float)glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
    slendermanShaderModel = glm::scale(slendermanShaderModel, glm::vec3(0.01f, 0.01f, 0.01f));
    return slendermanShaderModel;
}

float SlenderManager::_calcSlenderRotationAngle(const Camera& camera) {
    float zProjection = abs(_slendermanTranslationVector.z - camera.Position.z);
    float slenderDistance = sqrt(pow(_slendermanTranslationVector.x - camera.Position.x, 2) + pow(_slendermanTranslationVector.z - camera.Position.z, 2));
    float rotationAngle = acos(zProjection / slenderDistance) * 180.0 / M_PI;
    if (camera.Position.z < _slendermanTranslationVector.z && camera.Position.x > _slendermanTranslationVector.x) {
        rotationAngle = 180.0f - rotationAngle;
    }
    if (camera.Position.z < _slendermanTranslationVector.z && camera.Position.x < _slendermanTranslationVector.x) {
        rotationAngle += 180.0f;
    }
    if (camera.Position.z > _slendermanTranslationVector.z && camera.Position.x < _slendermanTranslationVector.x) {
        rotationAngle = -rotationAngle;
    }
    return rotationAngle;
}

vector<glm::vec3> SlenderManager::_getNearSpawnPoints(Camera camera, std::vector<glm::vec3> slendermanSpawnPoints, const int collectedPages) {
    vector<glm::vec3> nearSpawnPoints;
    for (int i = 0; i < slendermanSpawnPoints.size(); i++) {
        float pointCameraDistance = sqrt(pow(slendermanSpawnPoints[i].x - camera.Position.x, 2) + pow(slendermanSpawnPoints[i].z - camera.Position.z, 2));
        // Check sulla distanza TODO: Aggiornare in base al numero delle pagine ed aggiungere distanza minima
        if (pointCameraDistance > (MAX_EXTERNAL_SPAWN_DISTANCE - SPAWN_OFFSET_PER_PAGE * collectedPages)) {
            continue;
        }
        if (pointCameraDistance < (MAX_INTERNAL_SPAWN_DISTANCE - SPAWN_OFFSET_PER_PAGE * collectedPages)) {
            continue;
        }
        // Check sull'angolo TODO: Verificare che il cono di 40 gradi vada bene
        float angle = atan2((slendermanSpawnPoints[i].x - camera.Position.x), (slendermanSpawnPoints[i].z - camera.Position.z)) - atan2(camera.Front.x, camera.Front.z);
        angle = angle * 180 / M_PI;
        if (angle < 0) {
            angle = angle + 360.0f;
        }
        if (angle >= HALF_CONE_OPENING && angle <= 360.0f - HALF_CONE_OPENING) {
            continue;
        }
        //cout << "x: " << slendermanSpawnPoints[i].x << " z: " << slendermanSpawnPoints[i].z << " angle: " << angle << endl;
        nearSpawnPoints.push_back(slendermanSpawnPoints[i]);
    }
    return nearSpawnPoints;
}

float SlenderManager::_calcNegativeFearFactor(float distance, float angle, float timeDifference) {
    float fearFactor = (1.0f / (MEDIUM_TIME_DEATH * 2)) * (distance) / DISTANCE_RESET_FEAR;
    if (angle <= 180.0f) {
        fearFactor -= fearFactor * (180.0f - angle) / 180.0f;
    }
    else {
        fearFactor -= fearFactor * (angle - 180.0f) / 180.0f;
    }
    return fearFactor * timeDifference;
}

float SlenderManager::_calcPositiveFearFactor(float distance, float angle, float timeDifference) {
    float fearFactor = (1.0f / MEDIUM_TIME_DEATH) * (DISTANCE_RESET_FEAR - distance) / DISTANCE_RESET_FEAR;
    if (angle <= HALF_SLENDER_CONE_OPENING) {
        fearFactor -= fearFactor * angle / (2 * HALF_SLENDER_CONE_OPENING);
    }
    if (angle >= 360.0f - HALF_SLENDER_CONE_OPENING) {
        fearFactor -= fearFactor * (360.0f - angle) / (2 * HALF_SLENDER_CONE_OPENING);
    }
    return fearFactor * timeDifference;
}

