
#pragma once

#include <cmath>
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
#include "../renderable_aabb.h"
#include "../renderable_poi.h"
#include "../render_text.h"
#include "../scene.h"
#include "../shader_cache.h"
#include "../slenderman.h"
#include "../street_light.h"
#include "../slender_manager.h"

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
    SlenderManager* _slenderManager;
    vector<Page*> _pages;

    void _updateInitInfo(std::string info);

    void _processInput(const float& deltaTime, const CollisionResult& collisionResult);
    void _findFramedPage();

    void _renderInfo();

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
    _slenderManager = new SlenderManager();
    _renderables.push_back(_slenderMan);

    _updateInitInfo("Generating random map...");
    _renderables.push_back(new DynamicMapRenderable(DynamicEntity::grass));

    unordered_set<int> tabooIndices = unordered_set<int>();
    for (int index : K_SET_TO_EXCLUDE)
        tabooIndices.insert(index);

    for (auto poi : _poiInfo)
        tabooIndices.insert(poi.first);


    _updateInitInfo("Generating random forest...");
    DynamicMapRenderable* forest = new DynamicMapRenderable(DynamicEntity::tree, tabooIndices);
    _renderables.push_back(forest);
    std::vector<aabb> forestAABBs = forest->toAABBs();
    //_collisionSolver.registerAABBs(forestAABBs);
    //for (auto forestAABB : forestAABBs)
        //_renderables.push_back(new RenderableAABB(forestAABB));

    aabb fenceFront = aabb(glm::vec3(MAX_PLAYER_DISTANCE_LEFT, -4.0f, MAX_PLAYER_DISTANCE_FRONT + 0.25f), glm::vec3(MAX_PLAYER_DISTANCE_RIGHT, 0.0f, MAX_PLAYER_DISTANCE_FRONT - 0.25f));
    _collisionSolver.registerAABB(fenceFront);
    _renderables.push_back(new RenderableAABB(fenceFront));
    aabb fenceBack = aabb(glm::vec3(MAX_PLAYER_DISTANCE_LEFT, -4.0f, MAX_PLAYER_DISTANCE_BACK + 0.25f), glm::vec3(MAX_PLAYER_DISTANCE_RIGHT, 0.0f, MAX_PLAYER_DISTANCE_BACK - 0.25f));
    _collisionSolver.registerAABB(fenceBack);
    _renderables.push_back(new RenderableAABB(fenceBack));
    aabb fenceRight = aabb(glm::vec3(MAX_PLAYER_DISTANCE_RIGHT - 5.25, -4.0f, MAX_PLAYER_DISTANCE_BACK), glm::vec3(MAX_PLAYER_DISTANCE_RIGHT + 5.25f, 0.0f, MAX_PLAYER_DISTANCE_FRONT));
    _collisionSolver.registerAABB(fenceRight);
    _renderables.push_back(new RenderableAABB(fenceRight));
    aabb fenceLeft = aabb(glm::vec3(MAX_PLAYER_DISTANCE_LEFT - 5.25, -4.0f, MAX_PLAYER_DISTANCE_BACK), glm::vec3(MAX_PLAYER_DISTANCE_LEFT + 5.25f, 0.0f, MAX_PLAYER_DISTANCE_FRONT));
    _collisionSolver.registerAABB(fenceLeft);
    _renderables.push_back(new RenderableAABB(fenceLeft));

    _renderables.push_back(new Fence());

    MapInitializer::addPOIRenderablesAndStreetLights(_poiInfo, _pages, _renderables, _collisionSolver);
    _renderables.push_back(new Minimap(_poiInfo));
}

void TestScene::_processInput(const float& deltaTime, const CollisionResult& collisionResult) {
    if (InputManager::isKeyPressed(GLFW_KEY_ESCAPE)) {
        glfwSetWindowShouldClose(_window, true);
        return;
    }

    bool superSayan = InputManager::isKeyPressed(GLFW_KEY_LEFT_SHIFT);
    float speedIncrement = superSayan ? 150.0f : 0.0f;

    if (InputManager::isKeyPressed(GLFW_KEY_W) && (!collisionResult.n || superSayan))
        _camera.ProcessKeyboard(FORWARD, deltaTime, speedIncrement);
    if (InputManager::isKeyPressed(GLFW_KEY_S) && (!collisionResult.s || superSayan))
        _camera.ProcessKeyboard(BACKWARD, deltaTime, speedIncrement);
    if (InputManager::isKeyPressed(GLFW_KEY_A) && (!collisionResult.w || superSayan))
        _camera.ProcessKeyboard(LEFT, deltaTime, speedIncrement);
    if (InputManager::isKeyPressed(GLFW_KEY_D) && (!collisionResult.e || superSayan))
        _camera.ProcessKeyboard(RIGHT, deltaTime, speedIncrement);

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

    _slenderManager->updateSlenderman(_camera, *_slenderMan);

    for (auto renderable : _renderables)
        renderable->render(_camera, _lightUtils);

    if (!_collectedPageMessage.empty() && _pageCollectedTime + PAGE_COLLECTED_MESSAGE_SECONDS > glfwGetTime())
        RenderText(_collectedPageMessage, (SCR_WIDTH / 2) - 150.0f, SCR_HEIGHT - 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    _renderInfo();
}

void TestScene::_renderInfo() {
    std::stringstream ssx;
    ssx << "x: " << _camera.Position.x;
    std::string x = ssx.str();
    RenderText(x, SCR_WIDTH - 200.0f, 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
    std::stringstream ssz;
    ssz << "z: " << _camera.Position.z;
    std::string z = ssz.str();
    RenderText(z, SCR_WIDTH - 200.0f, 70.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    //int numVAOForSide = TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_TREE;
    //vector<int> indexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE);
    //int k_index = indexes[(indexes.size() - 1) / 2];
    //int x_index = k_index / numVAOForSide;
    //int z_index = k_index % numVAOForSide;

    //std::stringstream ssk;
    //ssk << "k_index: " << k_index;
    //std::string k = ssk.str();
    //RenderText(k, SCR_WIDTH - 200.0f, 120.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    //std::stringstream ssxindex;
    //ssxindex << "x_index: " << x_index;
    //std::string x_ind = ssxindex.str();
    //RenderText(x_ind, SCR_WIDTH - 200.0f, 140.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    //std::stringstream sszindex;
    //sszindex << "z_index: " << z_index;
    //std::string z_ind = sszindex.str();
    //RenderText(z_ind, SCR_WIDTH - 200.0f, 160.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    std::stringstream ssfrontinfo;
    ssfrontinfo << "x_v: " << _camera.Front.x << " y_v: " << _camera.Front.y << " z_v: " << _camera.Front.z;
    std::string front_info = ssfrontinfo.str();
    RenderText(front_info, 100.0f, 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    /*std::stringstream ssCollectPage;
    ssCollectPage << "Viewed Page: " << posViewedPage;
    std::string collectPage_str = ssCollectPage.str();
    RenderText(collectPage_str, 50.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));*/
}

void TestScene::destroy() {
    for (auto renderable : _renderables)
        delete renderable;
    _renderables.clear();
}

Camera* TestScene::currentCamera() {
    return &_camera;
}