#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/detail/type_vec.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "camera.h"
#include <cmath>

class SlenderManager {

public:
    double previousTime;
    glm::vec3 _slendermanTranslationVector;

    SlenderManager() {
        previousTime = glfwGetTime();
        _slendermanTranslationVector = glm::vec3(0.0f, -0.8f, -10.0f);
    }

    glm::mat4 getSlendemanShaderModel(Camera camera) {
        glm::mat4 slendermanShaderModel = glm::mat4(1.0f);
        slendermanShaderModel = glm::translate(slendermanShaderModel, _slendermanTranslationVector);
        float rotationAngle = _calcSlenderRotationAngle(camera);
        slendermanShaderModel = glm::rotate(slendermanShaderModel, (float)glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        slendermanShaderModel = glm::scale(slendermanShaderModel, glm::vec3(0.01f, 0.01f, 0.01f));
        return slendermanShaderModel;
    }

private:
    float _calcSlenderRotationAngle(Camera camera) {
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
};


