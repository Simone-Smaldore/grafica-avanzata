#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <thread>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

#include "test_scene.h"
#include "../constants.h"
#include "../game.h"
#include "../input_manager.h"
#include "../model_cache.h"
#include "../scene.h"
#include "../shader_cache.h"
#include "../texture_cache.h"

static void _forceRenderText(std::string text) {
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    RenderText("SLENDERMAN", 0, SCR_HEIGHT - 32, 1, glm::vec3(1, 1, 1));

    RenderText(text, 0, SCR_HEIGHT - 72, 0.8, glm::vec3(1, 1, 1));
    glfwSwapBuffers(_window);
}

class LoadingScene : public Scene {
private:
    TestScene* _gameScene;
    GLFWwindow* _window;
    SceneManager* _sceneManager;

    void _loadShaders();
    void _loadTextures();
    void _loadModels();

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

void LoadingScene::init() {
    initRenderText(SCR_WIDTH, SCR_HEIGHT);

    _forceRenderText("Loading shaders...");
    _loadShaders();
    _forceRenderText("Loading textures...");
    _loadTextures();
    _forceRenderText("Loading models...");
    _loadModels();
    _forceRenderText("Generating map...");

    _gameScene = new TestScene(_sceneManager, _forceRenderText);
    _gameScene->init();
}

void LoadingScene::process(const float& deltaTime) {
    RenderText("SLENDERMAN", 0, SCR_HEIGHT - 32, 1, glm::vec3(1, 1, 1));
    RenderText("Loading done: press space to play", 0, SCR_HEIGHT - 72, 0.8, glm::vec3(1, 1, 1));

    if (InputManager::isKeyPressed(GLFW_KEY_SPACE))
        _sceneManager->changeScene(_gameScene);
}

void LoadingScene::destroy() { }

Camera* LoadingScene::currentCamera() {
    return nullptr;
}