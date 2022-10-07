#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "fps_manager.h"
#include "input_manager.h"

#include "constants.h"
#include "model.h"
#include "model_cache.h"
#include "render_text.h"
#include "scene.h"
#include "scene/test_scene.h"
#include "shader_m.h"
#include "shader_cache.h"
#include "texture_cache.h"

class GameLoop {
private:
    FpsManager _fpsManager;

    GLFWwindow* _window;
    Scene* _currentScene;

    void _renderFPS();

public:
    GameLoop(GLFWwindow* window) : _window(window) {}

    void init();

    void process();

    void destroy();
};

void GameLoop::init() {
    _currentScene = new TestScene();
    InputManager::init(_window, _currentScene->currentCamera());

    initRenderText(SCR_WIDTH, SCR_HEIGHT);

    glEnable(GL_DEPTH_TEST);

    // nTODO gli Shader / Texture / Modelli possono essere caricati separatamente in una scena "Loading"
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

    TextureCache::getInstance().registerTexture(ETexture::slenderMan, "resources/models/Slenderman/diffuse.png");
    TextureCache::getInstance().registerTexture(ETexture::floor, "resources/textures/floor/floor.jpg");
    TextureCache::getInstance().registerTexture(ETexture::streetLight, "resources/models/Streetlight/streetlight_default_color.tga.png");
    TextureCache::getInstance().registerTexture(ETexture::minimap, "resources/textures/minimappa/bosco_dark2.jpg");
    TextureCache::getInstance().registerTexture(ETexture::fence, "resources/models/Fence/wood-fence/textura_cerca_de_madeira_COLOR.png");

    ModelCache::getInstance().registerModel(EModel::slenderMan, new Model("resources/models/Slenderman/Slenderman.obj"));
    ModelCache::getInstance().registerModel(EModel::streetLight, new Model("resources/models/Streetlight/streetlight.obj"));
    ModelCache::getInstance().registerModel(EModel::tree, new Model("resources/models/Tree/oaktrees.obj"));
    ModelCache::getInstance().registerModel(EModel::grass, new Model("resources/models/Grass/scene.gltf"));
    ModelCache::getInstance().registerModel(EModel::fence, new Model("resources/models/Fence/wood-fence/wood-fence.obj"));
    
    int poi1TextureEnumIndex = static_cast<int>(ETexture::poi1);
    int poi8TextureEnumIndex = static_cast<int>(ETexture::poi8);
    for (int poiEnumIndex = poi1TextureEnumIndex; poiEnumIndex <= poi8TextureEnumIndex; poiEnumIndex++) {
        int poiIndex = (poiEnumIndex - poi1TextureEnumIndex) + 1;
        std::string texturePath = "resources/models/Points of interest/" + std::to_string(poiIndex) + "/" + std::to_string(poiIndex) + ".jpg";
        TextureCache::getInstance().registerTexture(static_cast<ETexture>(poiEnumIndex), texturePath.c_str());
    }

    const vector<std::string> extensions = { ".dae", ".gltf", ".gltf", ".gltf", ".gltf", ".gltf", ".gltf", ".gltf" };
    int poi1ModelEnumIndex = static_cast<int>(EModel::poi1);
    int poi8ModelEnumIndex = static_cast<int>(EModel::poi8);
    for (int poiEnumIndex = poi1ModelEnumIndex; poiEnumIndex <= poi8ModelEnumIndex; poiEnumIndex++) {
        int poiIndex = (poiEnumIndex - poi1ModelEnumIndex) + 1;
        std::string modelPath = "resources/models/Points of interest/" + std::to_string(poiIndex) + "/" + std::to_string(poiIndex) + extensions[poiIndex - 1];
        ModelCache::getInstance().registerModel(static_cast<EModel>(poiEnumIndex), new Model(modelPath));
    }

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

        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        if (InputManager::isKeyPressed(GLFW_KEY_Q))
            glfwSetWindowShouldClose(_window, true);

        _renderFPS();

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

void GameLoop::_renderFPS() {
    std::stringstream ssfps;
    ssfps << "fps: " << _fpsManager.getFps();
    std::string fps_str = ssfps.str();
    RenderText(fps_str, SCR_WIDTH - 200.0f, SCR_HEIGHT - 50.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}