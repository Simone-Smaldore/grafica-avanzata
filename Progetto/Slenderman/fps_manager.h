#pragma once
#include <GLFW/glfw3.h>

struct FpsManager {
    int fps;
    int frameCount;
    double previousTime;

    FpsManager() : previousTime(glfwGetTime()), fps(0), frameCount(0) {}

    int getFps() {
        double currentTime = glfwGetTime();
        double timeInterval = currentTime - previousTime;

        frameCount++;

        if (timeInterval > 1.0f) {
            fps = frameCount;
            previousTime = currentTime;
            frameCount = 0;
        }

        return fps;
    }

};
