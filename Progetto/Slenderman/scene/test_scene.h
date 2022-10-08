#pragma once

#include <map>
#include <string>
#include <unordered_set>
#include <vector>

#include <glm/glm.hpp>

#include "../camera.h"
#include "../collision_solver.h"
#include "../constants.h"
#include "../dynamic_map_renderable.h"
#include "../fence.h"
#include "../floor.h"
#include "../input_manager.h"
#include "../light_utils.h"
#include "../map_initializer.h"
#include "../minimap.h"
#include "../model_cache.h"
#include "../texture_cache.h"
#include "../renderable_poi.h"
#include "../render_text.h"
#include "../scene.h"
#include "../shader_cache.h"
#include "../slenderman.h"
#include "../street_light.h"

typedef void (*initInfoCallback)(std::string info);

class TestScene : public Scene {
private:
    SceneManager* _sceneManager;
    initInfoCallback _infoCallback = nullptr;

    vector<Renderable*> _renderables;

    Camera _camera;
    LightUtils _lightUtils;
    CollisionSolver _collisionSolver;
    double _previousTime = 0.0;

    Page* _pageFramed = nullptr;
    int _collectedPages = 0;
    double _pageCollectedTime = 0.0;
    std::string _collectedPageMessage;

    // { poiKIndex - poiTranslation }
    std::map<int, glm::vec3> _poiInfo;
    std::unordered_set<int>* _tabooIndices;

    SlenderMan* _slenderMan;
    vector<Page*> _pages;

    void _updateInitInfo(std::string info);

    void _processInput(const float& deltaTime, const CollisionResult& collisionResult);
    void _findFramedPage();

public:
    TestScene(SceneManager* sceneManager, initInfoCallback infoCallback = nullptr) : _sceneManager{ sceneManager }, _infoCallback{ infoCallback } {}

    virtual void init() override;

    virtual void process(const float& deltaTime) override;

    virtual void destroy() override;

    inline virtual Camera* currentCamera() override;
};

void TestScene::_updateInitInfo(std::string info) {
    if (_infoCallback != nullptr)
        _infoCallback(info);
}

void TestScene::init() {
    _updateInitInfo("Generating POI...");
    _poiInfo = MapInitializer::initPOI();

    _lightUtils = LightUtils(_poiInfo);

    _renderables.push_back(new Floor());

    _slenderMan = new SlenderMan();
    _renderables.push_back(_slenderMan);

    _updateInitInfo("Generating random map...");
    _renderables.push_back(new DynamicMapRenderable(DynamicEntity::grass));

    unordered_set<int> tabooIndices = unordered_set<int>();
    for (int index : K_SET_TO_EXCLUDE)
        tabooIndices.insert(index);

    for (auto poi : _poiInfo)
        tabooIndices.insert(poi.first);

    MapInitializer::addPOIRenderablesAndStreetLights(_poiInfo, _pages, _renderables, _collisionSolver);

    _updateInitInfo("Generating random forest...");
    DynamicMapRenderable* forest = new DynamicMapRenderable(DynamicEntity::tree, tabooIndices);
    _renderables.push_back(forest);
    _collisionSolver.registerAABBs(forest->toAABBs());

    _renderables.push_back(new Fence());
    _renderables.push_back(new Minimap(_poiInfo));
}

void TestScene::_processInput(const float& deltaTime, const CollisionResult& collisionResult) {
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    if (InputManager::isKeyPressed(GLFW_KEY_W) && !collisionResult.n)
        _camera.ProcessKeyboard(FORWARD, deltaTime);
    if (InputManager::isKeyPressed(GLFW_KEY_S) && !collisionResult.s)
        _camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (InputManager::isKeyPressed(GLFW_KEY_A) && !collisionResult.w)
        _camera.ProcessKeyboard(LEFT, deltaTime);
    if (InputManager::isKeyPressed(GLFW_KEY_D) && !collisionResult.e)
        _camera.ProcessKeyboard(RIGHT, deltaTime);

    if (InputManager::isKeyPressed(GLFW_KEY_F)) {
        double currentTime = glfwGetTime();
        if (currentTime - _previousTime > 0.3f) {
            _previousTime = currentTime;
            _lightUtils.lightOn = !_lightUtils.lightOn;
        }
    }

    if (InputManager::isLeftMouseButtonPressed() && _pageFramed != nullptr && !_pageFramed->isCollected()) {
        _pageFramed->setCollected(true);
        _collectedPages++;
        _pageCollectedTime = glfwGetTime();

        std::stringstream ssPageInfo;
        ssPageInfo << "Collected Page: " << _collectedPages << "/" << NUM_PAGES;
        _collectedPageMessage = ssPageInfo.str();

        // nTODO: Vittoria
    }
}

void TestScene::_findFramedPage() {
    for (auto page : _pages) {
        page->setFramed(false);

        const glm::vec3& translationVec = page->getRelatedPOITranslation();
        float streetlampX = translationVec.x + STREETLIGHT_POI_OFFSET;
        float streetlightZ = translationVec.z + STREETLIGHT_POI_OFFSET;

        float streetlightDistance = sqrt(pow(streetlampX - _camera.Position.x, 2) + pow(streetlightZ - _camera.Position.z, 2));
        bool collectPosition = (_camera.Position.x > streetlampX && streetlightDistance <= PAGE_SELECTION_DISTANCE);

        float z = streetlightZ - _camera.Position.z;
        float alpha = z / streetlightDistance;
        bool collectVision = (_camera.Front.x > X_V_MIN_PAGE && _camera.Front.x < X_V_MAX_PAGE)
            && (_camera.Front.y > Y_V_MIN_PAGE && _camera.Front.y < Y_V_MAX_PAGE)
            && (_camera.Front.z > (Z_V_MIN_PAGE + alpha) && _camera.Front.z < (Z_V_MAX_PAGE + alpha))
            && (abs(alpha) < MAX_ANGLE_PAGE);

        if (collectPosition && collectVision) {
            page->setFramed(true);
            _pageFramed = page;
            return;
        }
    }

    _pageFramed = nullptr;
}

void TestScene::process(const float& deltaTime) {
    CollisionResult collisionResult = _collisionSolver.checkCollisionWithRegisteredAABBs(_camera, fmaxf(5.0f, (deltaTime * _camera.MovementSpeed) + 0.5f));
    _processInput(deltaTime, collisionResult);

    _findFramedPage();

    for (auto renderable : _renderables)
        renderable->render(_camera, _lightUtils);

    if (!_collectedPageMessage.empty() && _pageCollectedTime + PAGE_COLLECTED_MESSAGE_SECONDS > glfwGetTime())
        RenderText(_collectedPageMessage, (SCR_WIDTH / 2) - 150.0f, SCR_HEIGHT - 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void TestScene::destroy() {
    for (auto renderable : _renderables)
        delete renderable;
    _renderables.clear();
}

Camera* TestScene::currentCamera() {
    return &_camera;
}