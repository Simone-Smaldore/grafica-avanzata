#pragma once

#include <map>

#include "camera.h"
#include "input_manager.h"

enum class EScene {
    menu,
    loading,
    game,
};

class Scene {
protected:
    Camera _camera;
    LightUtils _lightUtils;

public:
    virtual void init() = 0;

    virtual void process(const float& deltaTime) = 0;

    virtual void destroy() = 0;

    inline virtual Camera* currentCamera() = 0;
};

class NullScene : public Scene {
    virtual void init() { };

    virtual void process(const float& deltaTime) override { };

    virtual void destroy() override { };

    inline virtual Camera* currentCamera() override { return nullptr; };
};

class SceneManager;

typedef Scene* SceneBuilder(SceneManager* sceneManager, GLFWwindow* window);

class SceneManager {
private:
    GLFWwindow* _window;

    NullScene* _nullScene = new NullScene();
    Scene* _currentScene = _nullScene;

    std::map<EScene, SceneBuilder*> _sceneBuilderMap;

public:
    SceneManager(GLFWwindow* window) : _window(window) {}

    inline Scene* currentScene() const;

    void addSceneBuilder(EScene scene, SceneBuilder* builder);

    void changeScene(EScene scene);
    
    void changePreloadedScene(Scene* scene);
};

Scene* SceneManager::currentScene() const {
    return _currentScene;
}

void SceneManager::addSceneBuilder(EScene scene, SceneBuilder* builder) {
    _sceneBuilderMap[scene] = builder;
}

void SceneManager::changeScene(EScene scene) {
    Scene* previousScene = _currentScene;

    _currentScene = (*_sceneBuilderMap[scene])(this, _window);
    _currentScene->init();

    previousScene->destroy();
    if (previousScene != _nullScene)
        delete previousScene;

    InputManager::bindCamera(_currentScene->currentCamera());
}

void SceneManager::changePreloadedScene(Scene* scene) {
    Scene* previousScene = _currentScene;
    _currentScene = _nullScene;

    previousScene->destroy();
    if (previousScene != _nullScene)
        delete previousScene;

    _currentScene = scene;
    InputManager::bindCamera(_currentScene->currentCamera());
}