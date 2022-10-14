#pragma once
#include "scene/loading_scene.h"
#include "fullscreen_image.h"


class MenuScene : public Scene {
private:
    LoadingScene* _loadingScene;
    GLFWwindow* _window;
    SceneManager* _sceneManager;
    FullsceenImage* _menuImage;

    bool _transitionStarted = false;
    double _transitionStartedTime;

    void _renderMenu();
public:
    MenuScene(SceneManager* sceneManager, GLFWwindow* window) : _sceneManager(sceneManager), _window(window) {}

    virtual void init() override;

    virtual void process(const float& deltaTime) override;

    virtual void destroy() override;

    inline virtual Camera* currentCamera() override;
};

void MenuScene::_renderMenu() {
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    _menuImage->render(_camera, _lightUtils);
    RenderText("[Enter] to Start Game", SCR_WIDTH / 2, SCR_HEIGHT / 3, 0.8, glm::vec3(1, 1, 1));
    RenderText("[Q] or [Esc] to Quit", SCR_WIDTH / 2, SCR_HEIGHT / 3 - 40, 0.8, glm::vec3(1, 1, 1));
    glfwSwapBuffers(_window);
    glfwSwapBuffers(_window);
}

void MenuScene::init() {
    initRenderText(SCR_WIDTH, SCR_HEIGHT);
    if (!ShaderCache::getInstance().has(EShader::fullScreenImage))
        ShaderCache::getInstance().registerShader(EShader::fullScreenImage, new Shader("minimap_shader.vs", "minimap_shader.fs"));
    if (!TextureCache::getInstance().has(ETexture::menuImage))
        TextureCache::getInstance().registerTexture(ETexture::menuImage, "resources/textures/menu_image.jpg");
    _menuImage = new FullsceenImage(ETexture::menuImage);
}

void MenuScene::process(const float& deltaTime) {
    _renderMenu();

    if (InputManager::isKeyPressed(GLFW_KEY_Q) || InputManager::isKeyPressed(GLFW_KEY_ESCAPE))
        glfwSetWindowShouldClose(_window, true);

    if (InputManager::isKeyPressed(GLFW_KEY_ENTER)) {
        _transitionStarted = true;
        _transitionStartedTime = glfwGetTime();
    }

    if (_transitionStarted && glfwGetTime() - _transitionStartedTime > 0.3) {
        _sceneManager->changeScene(EScene::loading);
    }
}

void MenuScene::destroy() { }

Camera* MenuScene::currentCamera() {
    return nullptr;
}