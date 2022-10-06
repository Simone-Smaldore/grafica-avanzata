#pragma once

#include <glm/glm.hpp>

#include "../camera.h"
#include "../constants.h"
#include "../floor.h"
#include "../light_utils.h"
#include "../model_cache.h"
#include "../texture_cache.h"
#include "../scene.h"
#include "../shader_cache.h"
#include "../slenderman.h"
#include "../street_light.h"

class TestScene : public Scene {
private:
    Camera _camera;
    LightUtils _lightUtils;

    SlenderMan* _slenderMan;
    Floor* _floor;
    StreetLight* _streetLight;

    void _processInput(const float& deltaTime);

public:
    virtual void init() override;

    virtual void process(const float& deltaTime) override;

    virtual void destroy() override;

    inline virtual Camera* currentCamera() override;
};

void TestScene::init() {
    _lightUtils = LightUtils({ glm::vec3(40.0f, -4.0f, 40.0f) });

    _slenderMan = new SlenderMan(_lightUtils);
    _floor = new Floor(_lightUtils);
    _streetLight = new StreetLight(_lightUtils);
}

void TestScene::_processInput(const float& deltaTime) {
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (InputManager::isKeyPressed(GLFW_KEY_W) /*&& !collisionResult.n*/)
        _camera.ProcessKeyboard(FORWARD, deltaTime);
    if (InputManager::isKeyPressed(GLFW_KEY_S) /*&& !collisionResult.s*/)
        _camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (InputManager::isKeyPressed(GLFW_KEY_A) /*&& !collisionResult.w*/)
        _camera.ProcessKeyboard(LEFT, deltaTime);
    if (InputManager::isKeyPressed(GLFW_KEY_D) /*&& !collisionResult.e*/)
        _camera.ProcessKeyboard(RIGHT, deltaTime);

    /*if (InputManager::isKeyPressed(GLFW_KEY_F)) {
        double currentTime = glfwGetTime();
        if (currentTime - previousTime > 0.3f) {
            previousTime = currentTime;
            lightOn = !lightOn;
        }
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && posViewedPage != -1) {
        collectedPagesIndices[posViewedPage] = true;
    }*/
}

void TestScene::process(const float& deltaTime) {
    _processInput(deltaTime);

    _slenderMan->render(_camera);
    _floor->render(_camera);
    _streetLight->render(_camera);
}

void TestScene::destroy() {
    delete _slenderMan;
    delete _floor;
    delete _streetLight;
}

Camera* TestScene::currentCamera() {
    return &_camera;
}