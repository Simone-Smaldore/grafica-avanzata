#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game.h"
#include "glfw_utils.h"

int main() {
    GLFWwindow* window = initGlfw();
    if (window == nullptr)
        return -1;

    GameLoop game(window);
    
    game.init();
    game.process();
    game.destroy();

    glfwTerminate();
    return 0;
}
