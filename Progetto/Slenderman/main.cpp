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
  Shader pageShader("model_loading.vs", "model_loading.fs");
  Shader forestShader("light_shader.vs", "light_shader.fs");
  Shader grassShader("light_shader.vs", "light_shader.fs");
  Shader fenceShader("light_shader.vs", "light_shader.fs");
  Shader streetlightShader("model_loading.vs", "model_loading.fs");
  Shader pointsOfInterestShader("model_loading.vs", "model_loading.fs");

  // Caricamento texture
  unsigned int slenderTexture = loadTexture("resources/models/Slenderman/diffuse.png");
  unsigned int flashlightTexture = loadTexture("resources/models/Torcia/DefaultMaterial_albedo.jpg");
  unsigned int floorTexture = loadTexture("resources/textures/floor/floor.jpg");
  unsigned int fenceTexture = loadTexture("resources/models/Fence/wood-fence/textura_cerca_de_madeira_COLOR.png");
  unsigned int streetlightTexture = loadTexture("resources/models/Streetlight/streetlight_default_color.tga.png");
  vector<unsigned int> pageTextures;
  for (int i = 1; i <= NUM_PAGES; i++) {
      std::string path = "resources/textures/Pages/page_"  + std::to_string(i) + ".jpg";
      pageTextures.push_back(loadTexture(path.c_str()));
  }
  vector<unsigned int> pointOfInterestTextures;
  for (int i = 1; i <= NUMBER_POINTS_OF_INTEREST; i++) {
      std::string path = "resources/models/Points of interest/" + std::to_string(i) + "/" + std::to_string(i) + ".jpg";
      pointOfInterestTextures.push_back(loadTexture(path.c_str()));
  }


  // Caricamento modelli
  Model slenderModel("resources/models/Slenderman/Slenderman.obj");
  Model flashlightModel("resources/models/Torcia/torcia.dae");
  Model treeModel("resources/models/Tree/oaktrees.obj");
  Model fenceModel("resources/models/Fence/wood-fence/wood-fence.obj");
  Model grassModel("resources/models/Grass/scene.gltf");
  Model streetlightModel("resources/models/Streetlight/streetlight.obj");
  vector<Model> pointsOfInterestModels;
  vector<std::string> poiModelExtensions{
      ".dae",
      ".gltf",
      ".dae",
      ".dae",
      ".dae",
      ".dae",
      ".dae",
      ".dae",
  };
  for (int i = 1; i <= NUMBER_POINTS_OF_INTEREST; i++) {
      std::string path = "resources/models/Points of interest/" + std::to_string(i) + "/" + std::to_string(i) + poiModelExtensions[i - 1];
      pointsOfInterestModels.push_back(Model(path.c_str()));
  }

  // Inizializzazione One-Time Scena
  unsigned int floorVAO;
  unsigned int pageVAO;
  vector<int> positionsPointOfinterest;
  vector<int> pageIndexPosition;
  vector<glm::vec3> pointOfinterestTranslationVec;
  //TODO: Spostare questi vector e trasformarli in matrici model da passare
  vector<glm::vec3> poiModelRotation{
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(1.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
  };
  vector<glm::vec3> poiModelScale{
    glm::vec3(0.008f, 0.008f, 0.008f),
    glm::vec3(0.03f, 0.03f, 0.03f),
    glm::vec3(0.008f, 0.008f, 0.008f),
    glm::vec3(0.008f, 0.008f, 0.008f),
    glm::vec3(0.008f, 0.008f, 0.008f),
    glm::vec3(0.008f, 0.008f, 0.008f),
    glm::vec3(0.008f, 0.008f, 0.008f),
    glm::vec3(0.008f, 0.008f, 0.008f),
  };
  vector<glm::vec3> poiModelTranslations{
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, -0.1f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 0.0f, 0.0f),
  };
  initScene(floorVAO, pageVAO, treeModel, fenceModel, grassModel, positionsPointOfinterest, pageIndexPosition, pointOfinterestTranslationVec);
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
    renderStreetlight(streetlightShader, streetlightTexture, streetlightModel, pointOfinterestTranslationVec, view, projection, lightOn);
    renderFlashlight(flashlightShader, flashlightTexture, flashlightModel, flashlightView, projection, lightOn);
    renderPages(pageShader, pageTextures, pageIndexPosition, pageVAO, pointOfinterestTranslationVec, view, projection, lightOn);
    renderPointsOfInterest(pointsOfInterestShader, pointsOfInterestModels, pointOfInterestTextures, pointOfinterestTranslationVec, view, projection, lightOn, poiModelRotation, poiModelScale, poiModelTranslations);
    if (DEBUG) renderInfo(camera, fps);

    // Swap dei buffer e processamento degli eventi in coda
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}



