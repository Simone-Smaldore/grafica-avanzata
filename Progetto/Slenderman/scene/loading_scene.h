#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <thread>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "game_scene.h"
#include "../audio_manager.h"
#include "../constants.h"
#include "../game.h"
#include "../input_manager.h"
#include "../model_cache.h"
#include "../scene.h"
#include "../shader_cache.h"
#include "../texture_cache.h"
#include "fullscreen_image.h"

class LoadingScene : public Scene {
private:
    GameScene* _gameScene;
    GLFWwindow* _window;
    SceneManager* _sceneManager;

    FullsceenImage* _menuImage;

    bool _transitionStarted = false;
    double _transitionStartedTime;

    void _loadShaders();
    void _loadTextures();
    void _loadModels();
    void _loadAudio();
    void _renderActualInfo(std::string text);

public:
    LoadingScene(SceneManager* sceneManager, GLFWwindow* window) : _sceneManager(sceneManager), _window(window) {}

    virtual void init() override;

    virtual void process(const float& deltaTime) override;

    virtual void destroy() override;

    inline virtual Camera* currentCamera() override;
};

void LoadingScene::_loadShaders() {
    ShaderCache::getInstance().registerShader(EShader::slenderMan, new Shader("multiple_lights.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::floor, new Shader("multiple_lights.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::streetLight, new Shader("multiple_lights.vs", "streetlight_shader.fs"));
    ShaderCache::getInstance().registerShader(EShader::tree, new Shader("multiple_lights_instancing.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::grass, new Shader("multiple_lights_instancing.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::poi, new Shader("multiple_lights.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::minimap, new Shader("minimap_shader.vs", "minimap_shader.fs"));
    ShaderCache::getInstance().registerShader(EShader::minimapWood, new Shader("minimap_shader.vs", "minimap_shader.fs"));
    ShaderCache::getInstance().registerShader(EShader::minimapCircle, new Shader("circle_minimap.vs", "circle_minimap.fs"));
    ShaderCache::getInstance().registerShader(EShader::fence, new Shader("multiple_lights_instancing.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::page, new Shader("multiple_lights.vs", "multiple_lights.fs"));
    ShaderCache::getInstance().registerShader(EShader::singleColor, new Shader("stencil_single_color.vs", "stencil_single_color.fs"));
    ShaderCache::getInstance().registerShader(EShader::aabb, new Shader("aabb.vs", "aabb.fs"));
    ShaderCache::getInstance().registerShader(EShader::fear, new Shader("fear.vs", "fear.fs"));
}

void LoadingScene::_loadTextures() {
    TextureCache::getInstance().registerTexture(ETexture::slenderMan, "resources/models/Slenderman/diffuse.png");
    TextureCache::getInstance().registerTexture(ETexture::floor, "resources/textures/floor/floor.jpg");
    TextureCache::getInstance().registerTexture(ETexture::streetLight, "resources/models/Streetlight/streetlight_default_color.tga.png");
    TextureCache::getInstance().registerTexture(ETexture::minimap, "resources/textures/minimappa/bosco_dark2.jpg");
    TextureCache::getInstance().registerTexture(ETexture::fence, "resources/models/Fence/wood-fence/textura_cerca_de_madeira_COLOR.png");

    int poi1TextureEnumIndex = static_cast<int>(ETexture::poi1);
    int poi8TextureEnumIndex = static_cast<int>(ETexture::poi8);
    for (int poiEnumIndex = poi1TextureEnumIndex; poiEnumIndex <= poi8TextureEnumIndex; poiEnumIndex++) {
        int poiIndex = (poiEnumIndex - poi1TextureEnumIndex) + 1;
        std::string texturePath = "resources/models/Points of interest/" + std::to_string(poiIndex) + "/" + std::to_string(poiIndex) + ".jpg";
        TextureCache::getInstance().registerTexture(static_cast<ETexture>(poiEnumIndex), texturePath.c_str());
    }

    int page1TextureEnumIndex = static_cast<int>(ETexture::page1);
    int page8TextureEnumIndex = static_cast<int>(ETexture::page8);
    for (int pageEnumIndex = page1TextureEnumIndex; pageEnumIndex <= page8TextureEnumIndex; pageEnumIndex++) {
        int pageIndex = (pageEnumIndex - page1TextureEnumIndex) + 1;
        std::string texturePath = "resources/textures/Pages/page_" + std::to_string(pageIndex) + ".jpg";;
        TextureCache::getInstance().registerTexture(static_cast<ETexture>(pageEnumIndex), texturePath.c_str());
    }
}

void LoadingScene::_loadModels() {
    ModelCache::getInstance().registerModel(EModel::slenderMan, new Model("resources/models/Slenderman/Slenderman.obj"));
    ModelCache::getInstance().registerModel(EModel::streetLight, new Model("resources/models/Streetlight/streetlight.obj"));
    ModelCache::getInstance().registerModel(EModel::tree, new Model("resources/models/Tree/oaktrees.obj"));
    ModelCache::getInstance().registerModel(EModel::grass, new Model("resources/models/Grass/scene.gltf"));
    ModelCache::getInstance().registerModel(EModel::fence, new Model("resources/models/Fence/wood-fence/wood-fence.obj"));

    const vector<std::string> extensions = { ".dae", ".gltf", ".gltf", ".gltf", ".gltf", ".gltf", ".gltf", ".gltf" };
    int poi1ModelEnumIndex = static_cast<int>(EModel::poi1);
    int poi8ModelEnumIndex = static_cast<int>(EModel::poi8);
    for (int poiEnumIndex = poi1ModelEnumIndex; poiEnumIndex <= poi8ModelEnumIndex; poiEnumIndex++) {
        int poiIndex = (poiEnumIndex - poi1ModelEnumIndex) + 1;
        std::string modelPath = "resources/models/Points of interest/" + std::to_string(poiIndex) + "/" + std::to_string(poiIndex) + extensions[poiIndex - 1];
        ModelCache::getInstance().registerModel(static_cast<EModel>(poiEnumIndex), new Model(modelPath));
    }
}

void LoadingScene::_loadAudio() {
    // TODO: load all audio
    //AudioManager::getInstance().loadMusic(EMusic::background, "resources/audio/creepy-music.mp3", true);
    AudioManager::getInstance().loadMusic(EMusic::whiteNoise, "resources/audio/white-noise.wav", true, 0.0f);

    AudioManager::getInstance().loadSfx(ESfx::lightOn, "resources/audio/light-on.wav");
}

void LoadingScene::_renderActualInfo(std::string text) {
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    _menuImage->render(_camera, _lightUtils);
    RenderText(text, SCR_WIDTH / 2, SCR_HEIGHT / 3 - 40, 0.8, glm::vec3(1, 1, 1));
    glfwSwapBuffers(_window);
}

void LoadingScene::init() {
    _menuImage = new FullsceenImage(ETexture::menuImage);
    initRenderText(SCR_WIDTH, SCR_HEIGHT);
    _renderActualInfo("Loading audio...");
    _loadAudio();
    _renderActualInfo("Loading shaders...");
    _loadShaders();
    _renderActualInfo("Loading textures...");
    _loadTextures();
    _renderActualInfo("Loading models...");
    _loadModels();
    _renderActualInfo("Generating map...");

    _gameScene = new GameScene(_sceneManager);
    _gameScene->init();
}

void LoadingScene::process(const float& deltaTime) {
    if (InputManager::isKeyPressed(GLFW_KEY_SPACE) && !_transitionStarted) {
        _transitionStarted = true;
        _transitionStartedTime = glfwGetTime();
    }

    if (!_transitionStarted) {
        _menuImage->render(_camera, _lightUtils);
        RenderText("Loading done: press space to play", SCR_WIDTH/2 - 400, 80, 0.8, glm::vec3(1, 1, 1));
    }

    if (_transitionStarted && glfwGetTime() - _transitionStartedTime > 1) {
        AudioManager::getInstance().playSfx(ESfx::lightOn, 0.33);
        _sceneManager->changeScene(_gameScene);
    }
}

void LoadingScene::destroy() { }

Camera* LoadingScene::currentCamera() {
    return nullptr;
}