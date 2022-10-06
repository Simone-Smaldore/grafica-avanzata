#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fps_manager.h"
#include "input_manager.h"

#include "model.h"
#include "model_cache.h"
#include "texture_cache.h"
#include "scene.h"
#include "scene/test_scene.h"
#include "shader_m.h"
#include "shader_cache.h"

class Game {
private:
    FpsManager _fpsManager;

    GLFWwindow* _window;
    Scene* _currentScene;

public:
    Game(GLFWwindow* window) : _window(window) {}

    void init();

    void process();

    void destroy();
};

void Game::init() {
    _currentScene = new TestScene();
    InputManager::init(_window, _currentScene->currentCamera());
    
    glEnable(GL_DEPTH_TEST);

    // nTODO gli Shader / Texture / Modelli possono essere caricati separatamente in una scena "Loading"
    Shader* slenderShader = new Shader("multiple_lights.vs", "multiple_lights.fs");
    ShaderCache::getInstance().registerShader(EShader::slenderMan, slenderShader);

    TextureCache::getInstance().registerTexture(ETexture::slenderMan, "resources/models/Slenderman/diffuse.png");

    Model* slenderModel = new Model("resources/models/Slenderman/Slenderman.obj");
    ModelCache::getInstance().registerModel(EModel::slenderMan, slenderModel);

    _currentScene->init();
}

void Game::process() {
    while (true) {
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        _currentScene->process();

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void Game::destroy() {
    _currentScene->destroy();
    delete _currentScene;

    ModelCache::getInstance().clear();
    ShaderCache::getInstance().clear();
    TextureCache::getInstance().clear();
}