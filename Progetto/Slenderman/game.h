#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fps_manager.h"
#include "input_manager.h"

#include "model.h"
#include "model_cache.h"
#include "texture_cache.h"
#include "scene/test_scene.h"
#include "shader_m.h"
#include "shader_cache.h"

class GameLoop {
private:
    FpsManager _fpsManager;

    GLFWwindow* _window;
    Scene* _currentScene;

public:
    GameLoop(GLFWwindow* window) : _window(window) {}

    void init();

    void process();

    void destroy();
};

void GameLoop::init() {
    _currentScene = new TestScene();
    InputManager::init(_window, _currentScene->currentCamera());

    glEnable(GL_DEPTH_TEST);

    // nTODO gli Shader / Texture / Modelli possono essere caricati separatamente in una scena "Loading"
    ShaderCache::getInstance().registerShader(EShader::slenderMan, new Shader("multiple_lights.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::floor, new Shader("multiple_lights.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::streetLight, new Shader("multiple_lights.vs", "streetlight_shader.fs"));

    TextureCache::getInstance().registerTexture(ETexture::slenderMan, "resources/models/Slenderman/diffuse.png");
    TextureCache::getInstance().registerTexture(ETexture::floor, "resources/textures/floor/floor.jpg");
    TextureCache::getInstance().registerTexture(ETexture::streetLight, "resources/models/Streetlight/streetlight_default_color.tga.png");

    ModelCache::getInstance().registerModel(EModel::slenderMan, new Model("resources/models/Slenderman/Slenderman.obj"));
    ModelCache::getInstance().registerModel(EModel::streetLight, new Model("resources/models/Streetlight/streetlight.obj"));

    _currentScene->init();
}

void GameLoop::process() {
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int fps = 0;

    while (!glfwWindowShouldClose(_window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        fps = _fpsManager.getFps();

        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (InputManager::isKeyPressed(GLFW_KEY_Q))
            glfwSetWindowShouldClose(_window, true);

        _currentScene->process(deltaTime);

        glfwSwapBuffers(_window);
        glfwPollEvents();
    }
}

void GameLoop::destroy() {
    _currentScene->destroy();
    delete _currentScene;

    ModelCache::getInstance().clear();
    ShaderCache::getInstance().clear();
    TextureCache::getInstance().clear();
}