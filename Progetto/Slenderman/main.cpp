#include "texture_utils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_m.h"
#include "camera.h"
#include "input_utils.h"
#include "model.h"
#include <iostream>
#include <vector>
#include <string>
#include "glfw_utils.h"
#include "scene.h"
#include "renderer.h"

int main() {
  // Inizializza Glfw
  GLFWwindow* window = initGlfw();
  if (window == nullptr) {
      return -1;
  }

  // Configura lo stato globale di opengl
  glEnable(GL_DEPTH_TEST);

  // Build e compilazione degli shader (TODO: Refactor finale shader)
  Shader slenderShader("model_loading.vs", "model_loading.fs");
  Shader flashlightShader("model_loading.vs", "model_loading.fs");
  Shader forestShader("forest.vs", "forest.fs");
  Shader floorShader("model_loading.vs", "model_loading.fs");
  Shader grassShader("grass.vs", "grass.fs");

  // Caricamento texture
  unsigned int slenderTexture = loadTexture("resources/models/Slenderman/diffuse.png");
  unsigned int flashlightTexture = loadTexture("resources/models/Torcia/DefaultMaterial_albedo.jpg");
  unsigned int floorTexture = loadTexture("resources/textures/floor/floor.jpg");
  unsigned int grassTexture = loadTexture("resources/textures/grass2.png");

  // Caricamento modelli
  Model slenderModel("resources/models/Slenderman/Slenderman.obj");
  Model flashlightModel("resources/models/Torcia/torcia.dae");
  Model treeModel("resources/models/Tree/oaktrees.obj");

  // Inizializzazione One-Time Scena
  unsigned int floorVAO;
  unsigned int grassVAO;
  initScene(floorVAO, treeModel, grassVAO);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // Loop di rendering
  // -----------
  while (!glfwWindowShouldClose(window)){
    // Calcolo dei frame
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // Gestione dell'input
    processInput(window, camera, deltaTime);

    // Pulizia dei buffer
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rendering della scena
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 flashlightView = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    glm::vec3 slendermanTranslationMatrix = glm::vec3(0.0f, -0.5f, -10.0f);
    
    renderFloor(floorShader, floorTexture, floorVAO, view, projection);
    renderForest(forestShader, treeModel, view, projection, camera);
    renderGrass(grassShader, grassTexture, grassVAO, view, projection);
    renderSlenderman(slenderShader, slenderTexture, slenderModel, slendermanTranslationMatrix, view, projection);
    renderFlashlight(flashlightShader, flashlightTexture, flashlightModel, flashlightView, projection);

    // Swap dei buffer e processamento degli eventi in coda
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}



