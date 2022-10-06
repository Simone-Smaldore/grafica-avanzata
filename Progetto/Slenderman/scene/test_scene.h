#pragma once

#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "../camera.h"
#include "../constants.h"
#include "../dynamic_map_renderable.h"
#include "../floor.h"
#include "../input_manager.h"
#include "../light_utils.h"
#include "../map_initializer.h"
#include "../model_cache.h"
#include "../texture_cache.h"
#include "../scene.h"
#include "../shader_cache.h"
#include "../slenderman.h"
#include "../street_light.h"

class TestScene : public Scene {
private:
    vector<Renderable*> _renderables;

    Camera _camera;
    LightUtils _lightUtils;
    double _previousTime = 0.0;

    // { poiKIndex - poiTranslation }
    std::map<int, glm::vec3> _poiInfo;
    std::unordered_set<int>* _tabooIndices;

    SlenderMan* _slenderMan;

    void _processInput(const float& deltaTime);

public:
    virtual void init() override;

    virtual void process(const float& deltaTime) override;

    virtual void destroy() override;

    inline virtual Camera* currentCamera() override;
};

void TestScene::init() {
    _poiInfo = MapInitializer::initPOI();

    _lightUtils = LightUtils(_poiInfo);

    _renderables.push_back(new Floor());

    _slenderMan = new SlenderMan();
    _renderables.push_back(_slenderMan);

    _renderables.push_back(new DynamicMapRenderable(DynamicEntity::grass));

    _tabooIndices = new unordered_set<int>();
    for (int index : K_MAP_TO_EXCLUDE)
        _tabooIndices->insert(index);

    for (auto poi : _poiInfo) {
        _tabooIndices->insert(poi.first);

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
        transform = glm::translate(transform, poi.second);
        transform = glm::scale(transform, glm::vec3(0.015f, 0.015f, 0.015f));
        _renderables.push_back(new StreetLight(transform));
    }

    _renderables.push_back(new DynamicMapRenderable(DynamicEntity::tree, _tabooIndices));
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

    if (InputManager::isKeyPressed(GLFW_KEY_F)) {
        double currentTime = glfwGetTime();
        if (currentTime - _previousTime > 0.3f) {
            _previousTime = currentTime;
            _lightUtils.lightOn = !_lightUtils.lightOn;
        }
    }

    /*
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && posViewedPage != -1) {
        collectedPagesIndices[posViewedPage] = true;
    }*/
}

void TestScene::process(const float& deltaTime) {
    _processInput(deltaTime);

    for (auto renderable : _renderables)
        renderable->render(_camera, _lightUtils);
}

void TestScene::destroy() {
    for (auto renderable : _renderables)
        delete renderable;
    _renderables.clear();
}

Camera* TestScene::currentCamera() {
    return &_camera;
}