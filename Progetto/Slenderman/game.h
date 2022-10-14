#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fps_manager.h"
#include "input_manager.h"

#include "audio_manager.h"
#include "constants.h"
#include "model.h"
#include "model_cache.h"
#include "raudio/raudio.h"
#include "render_text.h"
#include "scene/loading_scene.h"
#include "scene/game_scene.h"
#include "menu_scene.h"
#include "shader_m.h"
#include "shader_cache.h"
#include "texture_cache.h"

class Scene;

class GameLoop {
private:
    FpsManager _fpsManager;

    GLFWwindow* _window;
    SceneManager* _sceneManager;
    
    static Scene* _buildMenuScene(SceneManager* manager, GLFWwindow* window) {
        return new MenuScene(manager, window);
    }

    static Scene* _buildLoadingScene(SceneManager* manager, GLFWwindow* window) {
        return new LoadingScene(manager, window);
    }

    static Scene* _buildGameScene(SceneManager* manager, GLFWwindow* window) {
        return new GameScene(manager);
    }
    
    void _renderFPS();

public:
    GameLoop(GLFWwindow* window) : _window(window) {}

    void init();

    void process();

    void destroy();
};

void GameLoop::init() {
    _sceneManager = new SceneManager(_window);

    _sceneManager->addSceneBuilder(EScene::menu, GameLoop::_buildMenuScene);
    _sceneManager->addSceneBuilder(EScene::loading, GameLoop::_buildLoadingScene);
    _sceneManager->addSceneBuilder(EScene::game, GameLoop::_buildGameScene);

    _sceneManager->changeScene(EScene::menu);

    InputManager::init(_window, _sceneManager->currentScene()->currentCamera());
    AudioManager::getInstance().initAudio();

    glEnable(GL_DEPTH_TEST);

    _sceneManager->currentScene()->init();
}

void GameLoop::process() {
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int fps = 0;

    while (!glfwWindowShouldClose(_window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        _renderFPS();

        _sceneManager->currentScene()->process(deltaTime);

        glfwSwapBuffers(_window);
        glfwPollEvents();

        AudioManager::getInstance().process();
    }
}

void GameLoop::destroy() {
    _sceneManager->currentScene()->destroy();
    delete _sceneManager->currentScene();
    delete _sceneManager;

    ModelCache::getInstance().clear();
    ShaderCache::getInstance().clear();
    TextureCache::getInstance().clear();

    AudioManager::getInstance().destroy();
}

void GameLoop::_renderFPS() {
    std::stringstream ssfps;
    ssfps << "fps: " << _fpsManager.getFps();
    std::string fps_str = ssfps.str();
    RenderText(fps_str, SCR_WIDTH - 200.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}