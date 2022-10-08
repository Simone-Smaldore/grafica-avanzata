#pragma once

#include "camera.h"
#include "input_manager.h"

class Scene {
public:
    virtual void init() = 0;

    virtual void process(const float& deltaTime) = 0;

    virtual void destroy() = 0;

    inline virtual Camera* currentCamera() = 0;
};

class NullScene : public Scene {
    virtual void init() { };

    virtual void process(const float& deltaTime) override { };

    virtual void destroy() override { };

    inline virtual Camera* currentCamera() override { return nullptr; };
};

class SceneManager {
private:
    NullScene* _nullScene = new NullScene();

    Scene* _currentScene = _nullScene;

public:
    inline Scene* currentScene() const;

    void changeScene(Scene* scene);
};

Scene* SceneManager::currentScene() const {
    return _currentScene;
}

void SceneManager::changeScene(Scene* scene) {
    Scene* previousScene = _currentScene;
    _currentScene = _nullScene;

    previousScene->destroy();
    if (previousScene != _nullScene)
        delete previousScene;

    _currentScene = scene;
    InputManager::bindCamera(_currentScene->currentCamera());
}

//#include "constants.h"
//#include <time.h>
//
////TODO Transformare in oggetto scene con metodi privati
//
//void initFloor(unsigned int& floorVAO);
//void initPage(unsigned int& pageVAO);
//void initTreeForest(Model& treeModel, vector<glm::mat4>* treeModels, const vector<int>& poiIndices);
//void initFence(Model& fenceModel);
//void initGrass(Model& grassModel);
//void initPointsOfInterest(vector<int>& positionsPointOfinterest, vector<glm::vec3>& pointOfinterestTranslationVec);
//void initPoiModels(vector<glm::mat4>& modelPoiMatrices, vector<glm::vec3>& pointOfinterestTranslationVec);
//void initPageIndexPosition(vector<int>& pageIndexPosition);
//void initMiniMap(unsigned int& minimapVAO, unsigned int& framebuffer, unsigned int& textureColorBuffer, unsigned int& minimapWoodVAO);
//void initCircleMinimap(unsigned int& circleVAO);
//
////Privati
//void initDynamicMapForModel(Model& model, int quadSide, int vaoObjectSide, float offset, glm::vec3& scaleMatrix, bool useRandomOffset, vector<glm::mat4>* modelTransforms = nullptr, const vector<int>& poiIndices = {});
//bool isGoodPointOfInterest(int k, vector<int>& positionsPointOfinterest, int kMax, int numVAOForSide);
//void initRectVAO(unsigned int& rectVAO, float dimension);
//
//
//void initScene(
//    unsigned int& floorVAO,
//    unsigned int& pageVAO,
//    Model& treeModel,
//    Model& fenceModel,
//    Model& grassModel,
//    vector<int>& positionsPointOfinterest,
//    vector<int>& pageIndexPosition,
//    vector<glm::vec3>& pointOfinterestTranslationVec,
//    vector<glm::mat4>& modelPoiMatrices,
//    // TODO: Allineare la strategia per riferimento (vettore vuoto)?
//    vector<glm::mat4>* treeModels,
//    unsigned int& minimapVAO,
//    unsigned int& framebuffer,
//    unsigned int& textureColorBuffer,
//    unsigned int& minimapWoodVAO,
//    unsigned int& circleVAO
//) {
//    initFloor(floorVAO);
//    initPage(pageVAO);
//    initFence(fenceModel);
//    initGrass(grassModel);
//    initPointsOfInterest(positionsPointOfinterest, pointOfinterestTranslationVec);
//    initTreeForest(treeModel, treeModels, positionsPointOfinterest);
//    initPoiModels(modelPoiMatrices, pointOfinterestTranslationVec);
//    initPageIndexPosition(pageIndexPosition);
//    initMiniMap(minimapVAO, framebuffer, textureColorBuffer, minimapWoodVAO);
//    initCircleMinimap(circleVAO);
//}
//
//void initFloor(unsigned int& floorVAO) {
//    initRectVAO(floorVAO, 2000.0f);
//}
//
//void initPage(unsigned int& pageVAO) {
//    initRectVAO(pageVAO, 1.0f);
//}
//
//void initTreeForest(Model& treeModel, vector<glm::mat4>* treeModels, const vector<int>& poiIndices) {
//    bool useOffset = false;
//    glm::vec3 scaleMatrix = glm::vec3(0.08f, 0.08f, 0.08f);
//    initDynamicMapForModel(treeModel, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_TREE, TREE_OFFSET, scaleMatrix, useOffset, treeModels, poiIndices);
//}
//
//void initFence(Model& fenceModel) {
//    int sides = 4;
//    int num_VAO = NUM_FENCES_FOR_SIDE * sides;
//    glm::mat4* modelMatrices = new glm::mat4[num_VAO];
//    srand(glfwGetTime()); // initialize random seed	
//    float offset = FENCE_OFFSET;
//    float center_distance = (NUM_FENCES_FOR_SIDE / 2) * FENCE_OFFSET;
//    for (int i = 0; i < NUM_FENCES_FOR_SIDE; i++) {
//        //FRONT
//        glm::mat4 model = glm::mat4(1.0f);
//        float x = (i - NUM_FENCES_FOR_SIDE / 2) * offset;
//        float y = -5.0f;
//        float z = -center_distance;
//        model = glm::translate(model, glm::vec3(x, y, z));
//        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
//        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
//        modelMatrices[i] = model;
//
//        //LEFT
//        model = glm::mat4(1.0f);
//        x = -center_distance - 9.0f;
//        z = ((i - NUM_FENCES_FOR_SIDE / 2)) * offset + 10.0f;
//        model = glm::translate(model, glm::vec3(x, y, z));
//        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
//        modelMatrices[NUM_FENCES_FOR_SIDE + i] = model;
//
//        //RIGHT
//        model = glm::mat4(1.0f);
//        x = center_distance - 9.0f;
//        z = ((i - NUM_FENCES_FOR_SIDE / 2)) * offset + 10.0f;
//        model = glm::translate(model, glm::vec3(x, y, z));
//        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
//        modelMatrices[(2 * NUM_FENCES_FOR_SIDE + i)] = model;
//
//        //BACK
//        model = glm::mat4(1.0f);
//        x = (i - NUM_FENCES_FOR_SIDE / 2) * offset;
//        z = center_distance - 1.0f;
//        model = glm::translate(model, glm::vec3(x, y, z));
//        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
//        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
//        modelMatrices[(3 * NUM_FENCES_FOR_SIDE + i)] = model;
//    }
//
//    for (int k = 0; k < num_VAO; k++) {
//        unsigned int buffer;
//        glGenBuffers(1, &buffer);
//        glBindBuffer(GL_ARRAY_BUFFER, buffer);
//        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &modelMatrices[k], GL_STATIC_DRAW);
//        for (unsigned int i = 0; i < fenceModel.meshes.size(); i++) {
//            unsigned int VAO;
//            glGenVertexArrays(1, &VAO);
//            glBindVertexArray(VAO);
//            // set attribute pointers for matrix (4 times vec4)
//            glEnableVertexAttribArray(3);
//            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//            glEnableVertexAttribArray(4);
//            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//            glEnableVertexAttribArray(5);
//            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//            glEnableVertexAttribArray(6);
//            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//            glVertexAttribDivisor(3, 1);
//            glVertexAttribDivisor(4, 1);
//            glVertexAttribDivisor(5, 1);
//            glVertexAttribDivisor(6, 1);
//
//            fenceModel.meshes[i].VAOs.push_back(VAO);
//            glBindVertexArray(0);
//
//        }
//    }
//
//    for (unsigned int i = 0; i < fenceModel.meshes.size(); i++) {
//        fenceModel.meshes[i].setupVAOs();
//    }
//}
//
//void initGrass(Model& grassModel) {
//    bool useOffset = true;
//    glm::vec3 scaleMatrix = glm::vec3(0.015f, 0.01f, 0.015f);
//    initDynamicMapForModel(grassModel, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS, GRASS_OFFSET, scaleMatrix, useOffset);
//}
//
//void initPointsOfInterest(vector<int>& positionsPointOfinterest, vector<glm::vec3>& pointOfinterestTranslationVec) {
//    srand(time(NULL));
//    int numVAOForSide = TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_TREE;
//    int kMax = (numVAOForSide * numVAOForSide) - 1;
//    for (int i = 0; i < NUMBER_POINTS_OF_INTEREST; i++) {
//        int k = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * kMax;
//        while (!isGoodPointOfInterest(k, positionsPointOfinterest, kMax, numVAOForSide)) {
//            k = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * kMax;
//        }
//
//        int x_index = k / numVAOForSide;
//        int z_index = k % numVAOForSide;
//        int offset_x_index = x_index - numVAOForSide / 2;
//        int offset_z_index = z_index - numVAOForSide / 2;
//        float x_offset = (TREE_OFFSET * VAO_OBJECTS_SIDE_TREE * offset_x_index) + TREE_OFFSET;
//        float z_offset = (TREE_OFFSET * VAO_OBJECTS_SIDE_TREE * offset_z_index) + TREE_OFFSET;
//        pointOfinterestTranslationVec.push_back(glm::vec3(x_offset, -4.0f, z_offset));
//        positionsPointOfinterest.push_back(k);
//    }
//
//}
//
//void initPoiModels(vector<glm::mat4>& modelPoiMatrices, vector<glm::vec3>& pointOfinterestTranslationVec) {
//    //1
//    glm::mat4 model = glm::mat4(1.0f);
//    model = glm::translate(model, pointOfinterestTranslationVec[0]);
//    model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));
//    modelPoiMatrices.push_back(model);
//
//    //2 
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
//    model = glm::translate(model, pointOfinterestTranslationVec[1]);
//    model = glm::scale(model, glm::vec3(0.03f, 0.03f, 0.03f));
//    model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
//    modelPoiMatrices.push_back(model);
//
//    //3
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
//    model = glm::translate(model, pointOfinterestTranslationVec[2]);
//    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
//    model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
//    modelPoiMatrices.push_back(model);
//
//    //4
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, -3.1f, 0.0f));
//    model = glm::translate(model, pointOfinterestTranslationVec[3]);
//    model = glm::scale(model, glm::vec3(8.5f, 8.5f, 8.5f));
//    modelPoiMatrices.push_back(model);
//
//    //5
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, 1.4f, 0.0f));
//    model = glm::translate(model, pointOfinterestTranslationVec[4]);
//    model = glm::scale(model, glm::vec3(0.05f, 0.05f, 0.05f));
//    modelPoiMatrices.push_back(model);
//
//    //6
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, pointOfinterestTranslationVec[5]);
//    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//    modelPoiMatrices.push_back(model);
//
//    //7
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
//    model = glm::translate(model, pointOfinterestTranslationVec[6]);
//    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
//    model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
//    modelPoiMatrices.push_back(model);
//
//    //8
//    model = glm::mat4(1.0f);
//    model = glm::translate(model, glm::vec3(-60.0f, 3.8f, 60.0f));
//    model = glm::translate(model, pointOfinterestTranslationVec[7]);
//    model = glm::scale(model, glm::vec3(0.008f, 0.008f, 0.008f));
//    model = glm::rotate(model, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
//    modelPoiMatrices.push_back(model);
//}
//
//void initPageIndexPosition(vector<int>& pageIndexPosition) {
//    for (int i = 0; i < NUM_PAGES; i++) {
//        int ranIndex = rand() % (NUMBER_POINTS_OF_INTEREST - 1);
//        while (std::count(pageIndexPosition.begin(), pageIndexPosition.end(), ranIndex)) {
//            ranIndex = rand() % (NUMBER_POINTS_OF_INTEREST - 1);
//        }
//        pageIndexPosition.push_back(ranIndex);
//    }
//}
//
//void initMiniMap(unsigned int& minimapVAO, unsigned int& framebuffer, unsigned int& textureColorBuffer, unsigned int& minimapWoodVAO) {
//    float minimapVertices[] = {
//        // positions                                                                                           // texCoords
//           1.0f - MAP_W_PROP_DIMENSION - MAP_W_PROP_OFFSET, -(1.0f - MAP_H_PROP_DIMENSION - MAP_H_PROP_OFFSET),  1.0f, 0.0f,
//           1.0f - MAP_W_PROP_DIMENSION - MAP_W_PROP_OFFSET, -1.0 + MAP_H_PROP_OFFSET                          ,  1.0f, 1.0f,
//           1.0f - MAP_W_PROP_OFFSET                       , -1.0 + MAP_H_PROP_OFFSET                          ,  0.0f, 1.0f,
//
//           1.0f - MAP_W_PROP_DIMENSION - MAP_W_PROP_OFFSET, -(1.0f - MAP_H_PROP_DIMENSION - MAP_H_PROP_OFFSET),  1.0f, 0.0f,
//           1.0f - MAP_W_PROP_OFFSET                       , -1.0 + MAP_H_PROP_OFFSET                          ,  0.0f, 1.0f,
//           1.0f - MAP_W_PROP_OFFSET                       , -(1.0f - MAP_H_PROP_DIMENSION - MAP_H_PROP_OFFSET),  0.0f, 0.0f
//    };
//    unsigned int minimapVBO;
//    glGenVertexArrays(1, &minimapVAO);
//    glGenBuffers(1, &minimapVBO);
//    glBindVertexArray(minimapVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, minimapVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(minimapVertices), &minimapVertices, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//
//
//    // framebuffer
//    glGenFramebuffers(1, &framebuffer);
//    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
//    // create a color attachment texture
//
//    glGenTextures(1, &textureColorBuffer);
//    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);
//    // create a renderbuffer object for depth and stencil attachment 
//    unsigned int rbo;
//    glGenRenderbuffers(1, &rbo);
//    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
//    // check funzionamento
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
//    glBindFramebuffer(GL_FRAMEBUFFER, 0);
//
//    // Inizializza il VAO per riempire la minimappa
//    float minimapWoodVertices[] = {
//        // positions   // texCoords
//        -1.0f,  1.0f,  0.0f, 1.0f,
//        -1.0f, -1.0f,  0.0f, 0.0f,
//         1.0f, -1.0f,  1.0f, 0.0f,
//
//        -1.0f,  1.0f,  0.0f, 1.0f,
//         1.0f, -1.0f,  1.0f, 0.0f,
//         1.0f,  1.0f,  1.0f, 1.0f
//    };
//
//    unsigned int minimapWoodVBO;
//    glGenVertexArrays(1, &minimapWoodVAO);
//    glGenBuffers(1, &minimapWoodVBO);
//    glBindVertexArray(minimapWoodVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, minimapWoodVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(minimapWoodVertices), &minimapWoodVertices, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
//}
//
//void initCircleMinimap(unsigned int& circleVAO) {
//    int steps = NUM_VERTICES_CIRCLE / 6;
//
//    float circleVertices[NUM_VERTICES_CIRCLE] = {};
//    float PI = 3.1416;
//    float angle = 2 * PI / steps;
//
//    float xPos = 0.0f;
//    float yPos = 0.0f;
//    float radius = 1.0f;
//
//    float prevX = xPos;
//    float prevY = yPos - radius;
//
//    for (int i = 1; i <= steps; i++) {
//        float newX = radius * (sin(angle * i));
//        float newY = -radius * (cos(angle * i));
//
//        int triangleIndex = (i - 1) * 6;
//        circleVertices[triangleIndex] = xPos;
//        circleVertices[triangleIndex + 1] = yPos;
//        circleVertices[triangleIndex + 2] = newX;
//        circleVertices[triangleIndex + 3] = newY;
//        circleVertices[triangleIndex + 4] = prevX;
//        circleVertices[triangleIndex + 5] = prevY;
//
//        prevX = newX;
//        prevY = newY;
//    }
//
//    unsigned int circleVBO;
//    glGenVertexArrays(1, &circleVAO);
//    glGenBuffers(1, &circleVBO);
//    glBindVertexArray(circleVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), &circleVertices, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//}
//
//void initDynamicMapForModel(Model& modelObj, int quadSide, int vaoObjectSide, float offset, glm::vec3& scaleMatrix, bool useRandomOffset, vector<glm::mat4>* modelTransforms, const vector<int>& poiIndices) {
//    int num_VAO = (quadSide / vaoObjectSide) * (quadSide / vaoObjectSide);
//
//    unsigned int amount = quadSide * quadSide;
//    vector<glm::mat4*> modelMatrices;
//    for (int k = 0; k < num_VAO; k++) {
//        modelMatrices.push_back(new glm::mat4[(amount / num_VAO)]);
//    }
//    srand(glfwGetTime()); // initialize random seed	
//    for (int i = 0; i < quadSide; i++) {
//        for (int j = 0; j < quadSide; j++) {
//            glm::mat4 model = glm::mat4(1.0f);
//            float rx = 0.0f;
//            float rz = 0.0f;
//            if (useRandomOffset) {
//                rx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * offset / 2;
//                rz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * offset / 2;
//            }
//
//            // 1. Traslazione degli alberi in base all'indice della griglia
//            float x = (i - quadSide / 2) * offset + rx;
//            float y = -4.0f;
//            float z = (j - quadSide / 2) * offset + rz;
//            model = glm::translate(model, glm::vec3(x, y, z));
//            // 2. Scala gli alberi
//            model = glm::scale(model, scaleMatrix);
//            // 3. Rotazione randomica
//            float rotAngle = (rand() % 360);
//            model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
//
//            // 4. Aggiunge al matrice al modello
//            unsigned int vao_i = floor(i / vaoObjectSide);
//            unsigned int vao_j = floor(j / vaoObjectSide);
//            unsigned int vao_index = (vao_i * (quadSide / vaoObjectSide)) + vao_j;
//            unsigned int matrix_index = ((i % vaoObjectSide) * vaoObjectSide) + (j % vaoObjectSide);
//            modelMatrices[vao_index][matrix_index] = model;
//
//            // TODO: utilizzare i set (set / unordered_set) per conservare gli indici
//            bool shouldPushTransform = modelTransforms != nullptr;
//            shouldPushTransform = shouldPushTransform &&
//                std::find(K_MAP_TO_EXCLUDE.begin(), K_MAP_TO_EXCLUDE.end(), vao_index) == K_MAP_TO_EXCLUDE.end();
//            shouldPushTransform = shouldPushTransform && !poiIndices.empty() &&
//                std::find(poiIndices.begin(), poiIndices.end(), vao_index) == poiIndices.end();
//            if (shouldPushTransform)
//                modelTransforms->push_back(model);
//        }
//    }
//
//    for (int k = 0; k < num_VAO; k++) {
//        unsigned int buffer;
//        glGenBuffers(1, &buffer);
//        glBindBuffer(GL_ARRAY_BUFFER, buffer);
//        glm::mat4* model = modelMatrices[k];
//        glBufferData(GL_ARRAY_BUFFER, (amount / num_VAO) * sizeof(glm::mat4), &model[0], GL_STATIC_DRAW);
//        for (unsigned int i = 0; i < modelObj.meshes.size(); i++) {
//            unsigned int VAO;
//            glGenVertexArrays(1, &VAO);
//            glBindVertexArray(VAO);
//            // set attribute pointers for matrix (4 times vec4)
//            glEnableVertexAttribArray(3);
//            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
//            glEnableVertexAttribArray(4);
//            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
//            glEnableVertexAttribArray(5);
//            glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
//            glEnableVertexAttribArray(6);
//            glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
//
//            glVertexAttribDivisor(3, 1);
//            glVertexAttribDivisor(4, 1);
//            glVertexAttribDivisor(5, 1);
//            glVertexAttribDivisor(6, 1);
//
//            modelObj.meshes[i].VAOs.push_back(VAO);
//            glBindVertexArray(0);
//
//        }
//    }
//
//    for (unsigned int i = 0; i < modelObj.meshes.size(); i++) {
//        modelObj.meshes[i].setupVAOs();
//    }
//}
//
//bool isGoodPointOfInterest(int k, vector<int>& positionsPointOfinterest, int kMax, int numVAOForSide) {
//    // Verifica che il punto non è già stato escluso a priori nella configurazione
//    if (std::find(K_MAP_TO_EXCLUDE.begin(), K_MAP_TO_EXCLUDE.end(), k) != K_MAP_TO_EXCLUDE.end()) {
//        return false;
//    }
//    // Esclude i 4 lati esterni
//    if (k < numVAOForSide || k >(kMax - numVAOForSide)) {
//        return false;
//    }
//    if (k % numVAOForSide == 0 || (k + 1) % numVAOForSide == 0) {
//        return false;
//    }
//    // Verifica che il punto non sia adiacente ad un altro punto già estratto
//    for (int i = 0; i < positionsPointOfinterest.size(); i++) {
//        int val = positionsPointOfinterest[i];
//        if (k + 1 == val || k - 1 == val) {
//            return false;
//        }
//        if (k + numVAOForSide == val || k + numVAOForSide + 1 == val || k + numVAOForSide - 1 == val) {
//            return false;
//        }
//        if (k - numVAOForSide == val || k - numVAOForSide + 1 == val || k - numVAOForSide - 1 == val) {
//            return false;
//        }
//    }
//    return true;
//}
//
//void initRectVAO(unsigned int& rectVAO, float dimension) {
//    float rectVertices[] = {
//        // positions            // normals         // texcoords
//         dimension, 0.0f,  dimension,  0.0f, 1.0f, 0.0f,  dimension,  0.0f,
//        -dimension, 0.0f,  dimension,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
//        -dimension, 0.0f, -dimension,  0.0f, 1.0f, 0.0f,   0.0f, dimension,
//
//         dimension, 0.0f,  dimension,  0.0f, 1.0f, 0.0f,  dimension,  0.0f,
//        -dimension, 0.0f, -dimension,  0.0f, 1.0f, 0.0f,   0.0f, dimension,
//         dimension, 0.0f, -dimension,  0.0f, 1.0f, 0.0f,  dimension, dimension
//    };
//
//    unsigned int rectVBO;
//    glGenVertexArrays(1, &rectVAO);
//    glGenBuffers(1, &rectVBO);
//    glBindVertexArray(rectVAO);
//    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);
//    glEnableVertexAttribArray(0);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(1);
//    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
//    glEnableVertexAttribArray(2);
//    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
//    glBindVertexArray(0);
//}