#pragma once
#include <cmath>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/detail/type_vec.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../camera.h"
#include "../slenderman.h"

class SlenderManager {
public:
    double _previousTime = glfwGetTime();
    glm::vec3 _slendermanTranslationVector = glm::vec3(0.0f, -15.8f, -10.0f);

    SlenderManager() {};

    void updateSlenderman(const Camera& camera, SlenderMan& slenderman, const std::vector<glm::vec3>& slendermanSpawnPoints);

private:
    glm::mat4 _getSlendemanShaderModel(const Camera& camera);

    float _calcSlenderRotationAngle(const Camera& camera);

    vector<glm::vec3> _getNearSpawnPoints(const Camera camera, const std::vector<glm::vec3> slendermanSpawnPoints);
};

void SlenderManager::updateSlenderman(const Camera& camera, SlenderMan& slenderman, const std::vector<glm::vec3>& slendermanSpawnPoints) {
    glm::mat4 slenderTransform = _getSlendemanShaderModel(camera);
    slenderman.setTransform(slenderTransform);

    //TODO Eliminare magic numbers (Secondi e distanza)
    if (glfwGetTime() - _previousTime > 3) {
        vector<glm::vec3> nearSpawnPoints = _getNearSpawnPoints(camera, slendermanSpawnPoints);
        if (nearSpawnPoints.empty()) {
            return;
        }
        int spawnPointIndex = rand() % nearSpawnPoints.size();
        //Scegliere randomicamente e con la direzione dello sguardo
        _slendermanTranslationVector = nearSpawnPoints[spawnPointIndex];
        _previousTime = glfwGetTime();
    }
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

vector<glm::vec3> SlenderManager::_getNearSpawnPoints(Camera camera, std::vector<glm::vec3> slendermanSpawnPoints) {
    vector<glm::vec3> nearSpawnPoints;
    for (int i = 0; i < slendermanSpawnPoints.size(); i++) {
        float pointCameraDistance = sqrt(pow(slendermanSpawnPoints[i].x - camera.Position.x, 2) + pow(slendermanSpawnPoints[i].z - camera.Position.z, 2));
        // Check sulla distanza TODO: Aggiornare in base al numero delle pagine ed aggiungere distanza minima
        if (pointCameraDistance > 80.0f) {
            continue;
        }
        // Check sull'angolo TODO: Verificare che il cono di 40 gradi vada bene
        float angle = atan2((slendermanSpawnPoints[i].x - camera.Position.x), (slendermanSpawnPoints[i].z - camera.Position.z)) - atan2(camera.Front.x , camera.Front.z);
        angle = angle * 180 / M_PI;
        if (angle < 0) {
            angle = angle + 360.0f;
        }
        if (angle >= 20.0f && angle <= 340.0f) {
            continue;
        }
        //cout << "x: " << slendermanSpawnPoints[i].x << " z: " << slendermanSpawnPoints[i].z << " angle: " << angle << endl;
        nearSpawnPoints.push_back(slendermanSpawnPoints[i]);
    }
    return nearSpawnPoints;
}

