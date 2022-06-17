#pragma once
#include <GLFW/glfw3.h>

class FpsManager {
public:

    int fps;
    int frameCount;
    double previousTime;
    
    FpsManager() {
        previousTime = glfwGetTime();
        fps = 0;
        frameCount = 0;
    }

    int getFps() {
		frameCount++;
		double currentTime = glfwGetTime();
		double timeInterval = currentTime - previousTime;
		if (timeInterval > 1.0f) {
			fps = frameCount;
			previousTime = currentTime;
			frameCount = 0;
		}
        return fps;
    }

   
};
