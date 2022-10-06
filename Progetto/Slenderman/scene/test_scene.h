#pragma once

#include <glm/glm.hpp>

#include "../camera.h"
#include "../constants.h"
#include "../light_utils.h"
#include "../model_cache.h"
#include "../texture_cache.h"
#include "../scene.h"
#include "../shader_cache.h"

class TestScene : public Scene {
private:
    Camera _camera;
    LightUtils _lightUtils;

public:

    virtual void init() override;

    virtual void process() override;

    virtual void destroy() override;

    virtual Camera* currentCamera() override;
};

void TestScene::init() { }

Camera* TestScene::currentCamera() {
    return &_camera;
}

void TestScene::process() {
    Shader* slenderShader = ShaderCache::getInstance().findShader(EShader::slenderMan);
    unsigned int slenderTexture = TextureCache::getInstance().findTexture(ETexture::slenderMan);
    Model* slenderModel = ModelCache::getInstance().findModel(EModel::slenderMan);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, slenderTexture);

    glm::mat4 view = _camera.GetViewMatrix();
    glm::mat4 flashlightView = glm::mat4(1.0f);
    float farClippingPlane = 300.0f;
    if (DEBUG) {
        farClippingPlane = 2000.0f;
    }
    glm::mat4 projection = glm::perspective(glm::radians(_camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, farClippingPlane);

    _lightUtils.initLightShader(*slenderShader, true, _camera);

    slenderShader->use();
    slenderShader->setMat4("view", view);
    slenderShader->setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.8f, -10.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    slenderShader->setMat4("model", model);
    slenderModel->Draw(*slenderShader);
}

void TestScene::destroy() { }