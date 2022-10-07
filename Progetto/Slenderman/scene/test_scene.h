#pragma once

#include <map>
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

class TestScene : public Scene {
private:
    vector<Renderable*> _renderables;

    Camera _camera;
    LightUtils _lightUtils;
    CollisionSolver _collisionSolver;
    double _previousTime = 0.0;
    Page* _pageFramed = nullptr;

    // { poiKIndex - poiTranslation }
    std::map<int, glm::vec3> _poiInfo;
    std::unordered_set<int>* _tabooIndices;

    SlenderMan* _slenderMan;
    vector<Page*> _pages;

    void _processInput(const float& deltaTime);
    void _findFramedPage();

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
    for (int index : K_SET_TO_EXCLUDE)
        _tabooIndices->insert(index);

    for (auto poi : _poiInfo)
        _tabooIndices->insert(poi.first);

    MapInitializer::addPOIRenderablesAndStreetLights(_poiInfo, _pages, _renderables);

    _renderables.push_back(new DynamicMapRenderable(DynamicEntity::tree, _tabooIndices));
    _renderables.push_back(new Fence());
    _renderables.push_back(new Minimap(_poiInfo));
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

    if (InputManager::isLeftMouseButtonPressed() && _pageFramed != nullptr) {
        _pageFramed->setCollected(true);
        // nTODO: Aumentare il numero di pagine collezionate / messaggio pagina collezionata
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
    _processInput(deltaTime);

    _findFramedPage();

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