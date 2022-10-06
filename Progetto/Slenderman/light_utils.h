#pragma once

#include <map>
#include <vector>

#include "camera.h"
#include "constants.h"
#include "shader_m.h"

class LightUtils {
public:
    LightUtils() = default;
    LightUtils(std::map<int, glm::vec3> poiInfo) {
        lightTranslationVec = std::vector<glm::vec3>();
        for (auto poi : poiInfo)
            lightTranslationVec.push_back(poi.second);
    }

    std::vector<glm::vec3> lightTranslationVec;
    bool lightOn = true;

    void initLightShader(Shader* shader, const Camera& camera) const;

private:
    void initSpotLight(Shader* shader, const Camera& camera) const;
    void initPointLightForPoi(Shader* shader, glm::vec3 basePosition, int lightIndex) const;
};


void LightUtils::initLightShader(Shader* shader, const Camera& camera) const {
    shader->use();
    initSpotLight(shader, camera);
    for (int i = 0; i < lightTranslationVec.size(); i++) {
        initPointLightForPoi(shader, lightTranslationVec[i], i);
    }
    shader->setFloat("material.shininess", 32.0f);
}

void LightUtils::initSpotLight(Shader* shader, const Camera& camera) const {
    shader->setVec3("spotLight.position", camera.Position);
    shader->setVec3("spotLight.direction", camera.Front);
    shader->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
    shader->setVec3("viewPos", camera.Position);

    // spotLight properties
    shader->setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
    // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
    // each environment and lighting type requires some tweaking to get the best out of your environment.
    shader->setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
    shader->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);

    if (!lightOn) {
        shader->setVec3("spotLight.ambient", 0.01f, 0.01f, 0.01f);
        shader->setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        shader->setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
    }

    shader->setFloat("spotLight.constant", 1.0f);
    shader->setFloat("spotLight.linear", 0.09f);
    shader->setFloat("spotLight.quadratic", 0.0032f);

    // TODO eliminare la possibilita di illuminare la scena in produzione
    if (ILLUMINATE_SCENE && !lightOn) {
        shader->setVec3("spotLight.ambient", 1.0f, 1.0f, 1.0f);
        shader->setFloat("spotLight.linear", 0.0f);
        shader->setFloat("spotLight.quadratic", 0.0f);
        shader->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(40.0f)));
    }
}

void LightUtils::initPointLightForPoi(Shader* shader, glm::vec3 basePosition, int lightIndex) const {
    glm::vec3 lightPos = basePosition + glm::vec3(STREETLIGHT_POI_OFFSET, 9.0f, STREETLIGHT_POI_OFFSET);
    shader->setVec3("pointLights[" + std::to_string(lightIndex) + "].position", lightPos);
    shader->setVec3("pointLights[" + std::to_string(lightIndex) + "].ambient", 1.0, 0.8, 0.0);
    shader->setVec3("pointLights[" + std::to_string(lightIndex) + "].diffuse", 1.0, 0.8, 0.0);
    shader->setVec3("pointLights[" + std::to_string(lightIndex) + "].specular", 1.0, 0.8, 0.0);
    shader->setFloat("pointLights[" + std::to_string(lightIndex) + "].constant", 1.0f);
    shader->setFloat("pointLights[" + std::to_string(lightIndex) + "].linear", 0.09);
    shader->setFloat("pointLights[" + std::to_string(lightIndex) + "].quadratic", 0.032);
}
