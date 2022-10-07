#pragma once

#include <GLFW/glfw3.h>

#include "constants.h"
#include "camera.h"

static float _lastX = 0;
static float _lastY = 0;
static bool _mouseMoved = false;

static GLFWwindow* _window = nullptr;
static Camera* _camera = nullptr;

class InputManager {
private:
    static void processMouse(GLFWwindow* window, double xPos, double yPos);

public:
    static void init(GLFWwindow* window, Camera* camera);
    
    inline static bool isKeyPressed(int key);

    inline static bool isLeftMouseButtonPressed();
};

void InputManager::init(GLFWwindow* window, Camera* camera) {
    _lastX = (float)SCR_WIDTH / 2.0;
    _lastY = (float)SCR_HEIGHT / 2.0;
    _mouseMoved = false;

    _window = window;
    _camera = camera;

    glfwSetCursorPosCallback(window, InputManager::processMouse);
}

void InputManager::processMouse(GLFWwindow* window, double xPos, double yPos) {
    if (!_mouseMoved) {
        _lastX = xPos;
        _lastY = yPos;
        _mouseMoved = true;
    }

    float xOffset = xPos - _lastX;
    float yOffset = _lastY - yPos;

    if (_camera != nullptr)
        _camera->ProcessMouseMovement(xOffset, yOffset);

    _lastX = xPos;
    _lastY = yPos;
}

bool InputManager::isKeyPressed(int key) {
    return glfwGetKey(_window, key) == GLFW_PRESS;
}

bool InputManager::isLeftMouseButtonPressed() {
    return glfwGetMouseButton(_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
}