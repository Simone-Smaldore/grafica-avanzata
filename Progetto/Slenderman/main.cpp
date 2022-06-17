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
#include "render_text.h"
#include "fps_manager.h"




int main() {
  // Inizializza Glfw
  GLFWwindow* window = initGlfw();
  if (window == nullptr) {
      return -1;
  }

  FpsManager* fpsManager = new FpsManager();

  initRenderText(SCR_WIDTH, SCR_HEIGHT);

  // Configura lo stato globale di opengl
  glEnable(GL_DEPTH_TEST);

  // Build e compilazione degli shader (TODO: Refactor finale shader)
  Shader slenderShader("model_loading.vs", "model_loading.fs");
  Shader flashlightShader("model_loading.vs", "model_loading.fs");
  Shader floorShader("model_loading.vs", "model_loading.fs");
  Shader forestShader("light_shader.vs", "light_shader.fs");
  Shader grassShader("light_shader.vs", "light_shader.fs");
  Shader fenceShader("light_shader.vs", "light_shader.fs");

  

  // Caricamento texture
  unsigned int slenderTexture = loadTexture("resources/models/Slenderman/diffuse.png");
  unsigned int flashlightTexture = loadTexture("resources/models/Torcia/DefaultMaterial_albedo.jpg");
  unsigned int floorTexture = loadTexture("resources/textures/floor/floor.jpg");
  unsigned int fenceTexture = loadTexture("resources/models/Fence/wood-fence/textura_cerca_de_madeira_COLOR.png");

  // Caricamento modelli
  Model slenderModel("resources/models/Slenderman/Slenderman.obj");
  Model flashlightModel("resources/models/Torcia/torcia.dae");
  Model treeModel("resources/models/Tree/oaktrees.obj");
  Model fenceModel("resources/models/Fence/wood-fence/wood-fence.obj");
  Model grassModel("resources/models/Grass/scene.gltf");

  // Inizializzazione One-Time Scena
  unsigned int floorVAO;
  unsigned int grassVAO;
  vector<int> positionsPointOfinterest;
  initScene(floorVAO, treeModel, fenceModel, grassModel, positionsPointOfinterest);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  int fps = 0;
  bool lightOn = true;

  cout << "Punti di interesse: " << endl;
  for (int i = 0; i < positionsPointOfinterest.size(); i++) {
      cout << "K " << i << ": " << positionsPointOfinterest[i] << endl;
  }

  // Loop di rendering
  // -----------
  while (!glfwWindowShouldClose(window)){
    // Calcolo dei frame
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    fps = fpsManager->getFps();

    // Gestione dell'input
    processInput(window, camera, deltaTime, lightOn);

    // Pulizia dei buffer
    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Rendering della scena
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 flashlightView = glm::mat4(1.0f);
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 2000.0f);

    glm::vec3 slendermanTranslationMatrix = glm::vec3(0.0f, -0.8f, -10.0f);
    
    //TODO: Aggiungere i punti di interesse
    //TODO: Aggiungere mappa?
    renderFloor(floorShader, floorTexture, floorVAO, view, projection, lightOn);
    renderForest(forestShader, treeModel, view, projection, camera, positionsPointOfinterest, lightOn);
    renderFence(fenceShader, fenceTexture, fenceModel, view, projection, lightOn);
    renderGrass(grassShader, grassModel, view, projection, camera, lightOn);
    renderSlenderman(slenderShader, slenderTexture, slenderModel, slendermanTranslationMatrix, view, projection, lightOn);
    renderFlashlight(flashlightShader, flashlightTexture, flashlightModel, flashlightView, projection, lightOn);
    if (DEBUG) renderInfo(camera, fps);

    // Swap dei buffer e processamento degli eventi in coda
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}



