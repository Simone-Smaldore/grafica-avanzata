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
#include "aabb.h"
#include "collision_solver.h"

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
    Shader shaderSingleColor("stencil_single_color.vs", "stencil_single_color.fs");
    Shader forestShader("multiple_lights_instancing.vs", "multiple_lights.fs");
    Shader grassShader("multiple_lights_instancing.vs", "multiple_lights.fs");
    Shader fenceShader("multiple_lights_instancing.vs", "multiple_lights.fs");
    Shader streetlightShader("multiple_lights.vs", "streetlight_shader.fs");
    Shader pointsOfInterestShader("multiple_lights.vs", "multiple_lights.fs");
    Shader minimapShader("minimap_shader.vs", "minimap_shader.fs");

    Shader minimapWoodShader("minimap_shader.vs", "minimap_shader.fs");
    Shader circleMinimapShader("circle_minimap.vs", "circle_minimap.fs");

    Shader aabbShader("aabb.vs", "aabb.fs");

    //minimapShader.use();
    //minimapShader.setInt("texture1", 0);

    // Caricamento texture
    unsigned int slenderTexture = loadTexture("resources/models/Slenderman/diffuse.png");
    unsigned int flashlightTexture = loadTexture("resources/models/Torcia/DefaultMaterial_albedo.jpg");
    unsigned int floorTexture = loadTexture("resources/textures/floor/floor.jpg");
    unsigned int fenceTexture = loadTexture("resources/models/Fence/wood-fence/textura_cerca_de_madeira_COLOR.png");
    unsigned int streetlightTexture = loadTexture("resources/models/Streetlight/streetlight_default_color.tga.png");
    vector<unsigned int> pageTextures;
    for (int i = 1; i <= NUM_PAGES; i++) {
        std::string path = "resources/textures/Pages/page_" + std::to_string(i) + ".jpg";
        pageTextures.push_back(loadTexture(path.c_str()));
    }
    vector<unsigned int> pointOfInterestTextures;
    for (int i = 1; i <= NUMBER_POINTS_OF_INTEREST; i++) {
        std::string path = "resources/models/Points of interest/" + std::to_string(i) + "/" + std::to_string(i) + ".jpg";
        pointOfInterestTextures.push_back(loadTexture(path.c_str()));
    }
    unsigned int woodMinimapTexture = loadTexture("resources/textures/minimappa/bosco_dark2.jpg");


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

    unsigned int minimapVAO;
    unsigned int framebuffer;
    unsigned int textureColorBuffer;
    unsigned int minimapWoodVAO;
    unsigned int circleVAO;

    initScene(floorVAO, pageVAO, treeModel, fenceModel, grassModel, positionsPointOfinterest, pageIndexPosition, pointOfinterestTranslationVec, modelPoiMatrices, minimapVAO, framebuffer, textureColorBuffer, minimapWoodVAO, circleVAO);
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    int fps = 0;
    bool lightOn = true;
    vector<bool> collectedPagesIndices(pageIndexPosition.size(), false);

    cout << "Punti di interesse: " << endl;
    for (int i = 0; i < pageIndexPosition.size(); i++) {
        cout << "K " << i << ": " << pageIndexPosition[i] << endl;
    }
    Renderer renderer = Renderer(pointOfinterestTranslationVec);

    for (int i = 0; i < pointOfinterestTranslationVec.size(); i++) {
        cout << "K " << i << ": " << "X: " << pointOfinterestTranslationVec[i].x << " Z: " << pointOfinterestTranslationVec[i].z << endl;
    }

    CollisionSolver collisionSolver;

    // AABB (es. lampione preso da modello) -> offset di 10.0f (costante) (40 + 10, 40 + 10)
    //aabb debugAABB = aabb::fromModel(streetlightModel);
    for (int i = 0; i < modelPoiMatrices.size(); i++) {
        aabb currentModelAABB = aabb::fromModel(pointsOfInterestModels[i], modelPoiMatrices[i]);
        if (DEBUG) {
            currentModelAABB.bindToVAO();
            cout << "currentModelAABB - VAO: " << currentModelAABB.vao() << endl;
        }
        collisionSolver.registerAABB(currentModelAABB);

        glm::mat4 transform(1);
        transform = glm::translate(transform, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
        transform = glm::translate(transform, pointOfinterestTranslationVec[i]);
        transform = glm::scale(transform, glm::vec3(0.015f, 0.015f, 0.015f));
        aabb currentStreetlightAABB = aabb::fromModel(streetlightModel, transform);
        if (DEBUG) {
            currentStreetlightAABB.bindToVAO();
            cout << "currentStreetlightAABB - VAO: " << currentStreetlightAABB.vao() << endl;
        }
        collisionSolver.registerAABB(currentStreetlightAABB);
    }


    // AABB alberi -> mini clustering dei vertici
    // Intersezioni -> 8 direzioni da passare alla camera

    // Loop di rendering
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // Calcolo dei frame
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        fps = fpsManager->getFps();

        CollisionResult collisionResult = collisionSolver.checkCollisionWithRegisteredAABBs(camera);

        // Gestione dell'input
        int numCollectedPages = count(collectedPagesIndices.begin(), collectedPagesIndices.end(), true);
        processInput(window, camera, deltaTime, lightOn, renderer.posViewedPage, collectedPagesIndices, collisionResult);

        // Pulizia dei buffer
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

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

        renderer.findLookingPage(camera, pageIndexPosition);


        //TODO: Aggiungere mappa?
        renderer.renderFloor(floorShader, floorTexture, floorVAO);
        renderer.renderForest(forestShader, treeModel, camera, positionsPointOfinterest);
        renderer.renderFence(fenceShader, fenceTexture, fenceModel);
        renderer.renderGrass(grassShader, grassModel, camera);
        renderer.renderSlenderman(slenderShader, slenderTexture, slenderModel, slendermanTranslationMatrix);
        renderer.renderStreetlight(streetlightShader, streetlightTexture, streetlightModel);
        renderer.renderPointsOfInterest(pointsOfInterestShader, pointsOfInterestModels, pointOfInterestTextures, modelPoiMatrices);
        renderer.renderPages(pageShader, shaderSingleColor, pageTextures, pageIndexPosition, collectedPagesIndices, pageVAO);

        // Renderizzare sempre come ultimo
        //renderer.renderFlashlight(flashlightShader, flashlightTexture, flashlightModel);

        // Renderizza il messaggio di pagina raccolta
        int actualCollectedPages = count(collectedPagesIndices.begin(), collectedPagesIndices.end(), true);
        if (numCollectedPages != actualCollectedPages) {
            renderer.renderPageTimer = glfwGetTime();
        }
        renderer.renderPageMessage(actualCollectedPages);

        renderer.buildMiniMap(framebuffer, minimapWoodShader, minimapWoodVAO, woodMinimapTexture, circleMinimapShader, circleVAO);
        renderer.renderMiniMap(minimapShader, minimapVAO, textureColorBuffer);

        if (DEBUG) {
            for (const auto& staticAABB : collisionSolver.registeredAABBs()) {
                renderer.renderAABB(staticAABB.vao(), aabbShader, staticAABB.hasIntersection() ? RED : AABB_COLOR);
            }

            renderer.renderInfo(camera, fps);
        }

        // Swap dei buffer e processamento degli eventi in coda
        glfwSwapBuffers(window);
        glfwPollEvents();

        if (actualCollectedPages == NUM_PAGES) {
            // TODO: SCHERMO VITTORIA
            cout << "HAI VINTO !!" << endl;
            break;
        }

    }

    glfwTerminate();
    return 0;
}



