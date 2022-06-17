#pragma once
#include <GLFW/glfw3.h>
#include "camera.h"

double previousTime = glfwGetTime();

void processInput(GLFWwindow* window, Camera& camera, float deltaTime, bool& lightOn) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        double currentTime = glfwGetTime();
        if (currentTime - previousTime > 0.3f) {
            previousTime = currentTime;
            lightOn = !lightOn;
        }
    }

        
}