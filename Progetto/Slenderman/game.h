#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fps_manager.h"
#include "input_manager.h"

#include "constants.h"
#include "model.h"
#include "model_cache.h"
#include "raudio/raudio.h"
#include "render_text.h"
#include "scene/loading_scene.h"
#include "scene/test_scene.h"
#include "shader_m.h"
#include "shader_cache.h"
#include "texture_cache.h"

class Scene;

class GameLoop {
private:
    FpsManager _fpsManager;

    GLFWwindow* _window;
    SceneManager* _sceneManager;

    Music backgroundMusic;

    void _renderFPS();

public:
    GameLoop(GLFWwindow* window) : _window(window) {}

    void init();

    void process();

    void destroy();
};

void GameLoop::init() {
    _sceneManager = new SceneManager();
    _sceneManager->changeScene(new LoadingScene(_sceneManager, _window));

    InputManager::init(_window, _sceneManager->currentScene()->currentCamera());

    glEnable(GL_DEPTH_TEST);

    InitAudioDevice();

    // nTODO: :) - Aggiungere AudioManager (singleton, gestisce rAudio)
    backgroundMusic = LoadMusicStream("resources/sounds/creepy-music.mp3");
    PlayMusicStream(backgroundMusic);

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

        if (InputManager::isKeyPressed(GLFW_KEY_Q))
            glfwSetWindowShouldClose(_window, true);

        _renderFPS();

        _sceneManager->currentScene()->process(deltaTime);

        glfwSwapBuffers(_window);
        glfwPollEvents();

        //UpdateMusicStream(backgroundMusic);
        if (GetMusicTimePlayed(backgroundMusic) > GetMusicTimeLength(backgroundMusic))
            SeekMusicStream(backgroundMusic, 0);
    }
}

void GameLoop::destroy() {
    _sceneManager->currentScene()->destroy();
    delete _sceneManager->currentScene();
    delete _sceneManager;

    ModelCache::getInstance().clear();
    ShaderCache::getInstance().clear();
    TextureCache::getInstance().clear();

    UnloadMusicStream(backgroundMusic);
    CloseAudioDevice();
}

void GameLoop::_renderFPS() {
    std::stringstream ssfps;
    ssfps << "fps: " << _fpsManager.getFps();
    std::string fps_str = ssfps.str();
    RenderText(fps_str, SCR_WIDTH - 200.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}