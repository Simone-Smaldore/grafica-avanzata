#pragma once

#include <glm/glm.hpp>

#include "../camera.h"
#include "../scene.h"

class TestScene : Scene {
public:
    Camera& camera;
    unsigned int texture;

    TestScene(Camera& c, unsigned int t) : camera{ c }, texture{ t } {}

    virtual void init() override;

    virtual void process() override;

    virtual void destroy() override;
};

void TestScene::init() {

}

void TestScene::process() {
    Shader* slenderShader = ShaderCache::getInstance().findShader(EShader::slenderMan);
    Model* slenderModel = ModelCache::getInstance().findModel(EModel::slenderMan);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 flashlightView = glm::mat4(1.0f);
    float farClippingPlane = 300.0f;
    if (DEBUG) {
        farClippingPlane = 2000.0f;
    }
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, farClippingPlane);

    //lightUtils.initLightShader(slenderShader, lightOn, camera);

    slenderShader->use();
    slenderShader->setMat4("view", view);
    slenderShader->setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.8f, -10.0f));
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    slenderShader->setMat4("model", model);
    slenderModel->Draw(*slenderShader);
}

void TestScene::destroy() {

}