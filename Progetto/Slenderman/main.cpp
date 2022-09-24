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
  Shader slenderShader("multiple_lights.vs", "multiple_lights.fs");
  Shader flashlightShader("multiple_lights.vs", "multiple_lights.fs");
  Shader floorShader("multiple_lights.vs", "multiple_lights.fs");
  Shader pageShader("multiple_lights.vs", "multiple_lights.fs");
  Shader forestShader("multiple_lights_instancing.vs", "multiple_lights.fs");
  Shader grassShader("multiple_lights_instancing.vs", "multiple_lights.fs");
  Shader fenceShader("multiple_lights_instancing.vs", "multiple_lights.fs");
  Shader streetlightShader("multiple_lights.vs", "streetlight_shader.fs");
  Shader pointsOfInterestShader("multiple_lights.vs", "multiple_lights.fs");

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
      ".gltf",
      ".gltf",
      ".gltf",
      ".gltf",
      ".gltf",
      ".gltf",
  };
  for (int i = 1; i <= NUMBER_POINTS_OF_INTEREST; i++) {
      std::string path = "resources/models/Points of interest/" + std::to_string(i) + "/" + std::to_string(i) + poiModelExtensions[i - 1];
      pointsOfInterestModels.push_back(Model(path.c_str()));
  }

  // Inizializzazione One-Time Scena
  unsigned int floorVAO;
  unsigned int pageVAO;
  vector<glm::mat4> modelPoiMatrices;
  vector<int> positionsPointOfinterest;
  vector<int> pageIndexPosition;
  vector<glm::vec3> pointOfinterestTranslationVec;

  initScene(floorVAO, pageVAO, treeModel, fenceModel, grassModel, positionsPointOfinterest, pageIndexPosition, pointOfinterestTranslationVec, modelPoiMatrices);
  float deltaTime = 0.0f;
  float lastFrame = 0.0f;
  int fps = 0;
  bool lightOn = true;

  cout << "Punti di interesse: " << endl;
  for (int i = 0; i < positionsPointOfinterest.size(); i++) {
      cout << "K " << i << ": " << positionsPointOfinterest[i] << endl;
  }
  Renderer renderer = Renderer(pointOfinterestTranslationVec);

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
    float farClippingPlane = 300.0f;
    if (DEBUG) {
        farClippingPlane = 2000.0f;
    }
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, farClippingPlane);

    glm::vec3 slendermanTranslationMatrix = glm::vec3(0.0f, -0.8f, -10.0f);

    renderer.lightOn = lightOn;
    renderer.view = view;
    renderer.projection = projection;

    // TODO: rimuovere magic numbers: Come ottengo la posizione del lampione?
    // Una volta ottenuta la posizione posso applicare gli offset definiti nelle costanti
    bool collectXZ = (camera.Position.x > 50 && camera.Position.x < 62) &&
        (camera.Position.z > 42 && camera.Position.z < 58);
    bool collectV = (camera.Front.x > X_V_MIN_PAGE && camera.Front.x < X_V_MAX_PAGE)
        && (camera.Front.y > Y_V_MIN_PAGE && camera.Front.y < Y_V_MAX_PAGE)
        && (camera.Front.z > Z_V_MIN_PAGE && camera.Front.z < Z_V_MAX_PAGE);
    bool collectPage = collectXZ && collectV;
    renderer.collectPage = collectPage;

    //TODO: Aggiungere mappa?
    renderer.renderFloor(floorShader, floorTexture, floorVAO);
    renderer.renderForest(forestShader, treeModel, camera, positionsPointOfinterest);
    renderer.renderFence(fenceShader, fenceTexture, fenceModel);
    renderer.renderGrass(grassShader, grassModel, camera);
    renderer.renderSlenderman(slenderShader, slenderTexture, slenderModel, slendermanTranslationMatrix);
    renderer.renderStreetlight(streetlightShader, streetlightTexture, streetlightModel);
    renderer.renderPages(pageShader, pageTextures, pageIndexPosition, pageVAO);
    renderer.renderPointsOfInterest(pointsOfInterestShader, pointsOfInterestModels, pointOfInterestTextures, modelPoiMatrices);
    // Renderizzare sempre come ultimo
    //renderer.renderFlashlight(flashlightShader, flashlightTexture, flashlightModel);
    
    
    if (DEBUG) renderer.renderInfo(camera, fps);

    // Swap dei buffer e processamento degli eventi in coda
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwTerminate();
  return 0;
}



