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



int main() {
  // Inizializza Glfw
  // ------------------------------
  GLFWwindow* window = initGlfw();
  if (window == nullptr) {
      return -1;
  }

  // Configura lo stato globale di opengl
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  // -------------------------
  // TODO: Refactor finale shader
  Shader slenderShader("model_loading.vs", "model_loading.fs");
  Shader torciaShader("model_loading.vs", "model_loading.fs");
  Shader forestShader("forest.vs", "forest.fs");
  Shader floorShader("model_loading.vs", "model_loading.fs");
  Shader grassShader("grass.vs", "grass.fs");

  // load textures
  // -------------
  unsigned int slenderTexture = loadTexture("resources/models/Slenderman/diffuse.png");
  unsigned int torciaTexture = loadTexture("resources/models/Torcia/DefaultMaterial_albedo.jpg");
  unsigned int floorTexture = loadTexture("resources/textures/floor/floor.jpg");
  unsigned int transparentTexture = loadTexture("resources/textures/grass2.png");

  // load models
  // -----------
  Model slenderModel("resources/models/Slenderman/Slenderman.obj");
  Model torciaModel("resources/models/Torcia/torcia.dae");
  Model treeModel("resources/models/Tree/oaktrees.obj");

  unsigned int floorVAO;
  unsigned int grassVAO;

  initScene(
     floorVAO, treeModel, grassVAO
  );

  unsigned int treeAmount = TREE_QUAD_SIDE * TREE_QUAD_SIDE;
  unsigned int grassAmount = GRASS_QUAD_SIDE * GRASS_QUAD_SIDE;

  float deltaTime = 0.0f;
  float lastFrame = 0.0f;

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window))
  {
    // per-frame time logic
    // --------------------
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // input
    // -----
    processInput(window, camera, deltaTime);

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // Model loading
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, slenderTexture);
    slenderShader.use();
    glm::mat4 view = camera.GetViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    slenderShader.setMat4("view", view);
    slenderShader.setMat4("projection", projection);
    glm::mat4 modelMesh = glm::mat4(1.0f);
    modelMesh = glm::translate(modelMesh, glm::vec3(1.0f, 0.0f, 8.0f));
    modelMesh = glm::scale(modelMesh, glm::vec3(0.01f, 0.01f, 0.01f));	
    slenderShader.setMat4("model", modelMesh);
    slenderModel.Draw(slenderShader);


    //TREE INSTANCE

    forestShader.use();
    forestShader.setMat4("projection", projection);
    forestShader.setMat4("view", view);

    // NON TOCCARE
    for (unsigned int i = 0; i < treeModel.meshes.size(); i++) {
        for (unsigned int j = 0; j < treeModel.meshes[i].textures.size(); j++) {
            glActiveTexture(GL_TEXTURE0 + j); 
            string name = treeModel.meshes[i].textures[j].type;
            glUniform1i(glGetUniformLocation(forestShader.ID, (name + std::to_string(j + 1)).c_str()), j);
            glBindTexture(GL_TEXTURE_2D, treeModel.meshes[i].textures[j].id);
        }
        glBindVertexArray(treeModel.meshes[i].VAO);
        glDrawElementsInstanced(GL_TRIANGLES, treeModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, treeAmount);
        glBindVertexArray(0);
    }

    //PAVIMENTO
    floorShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -4.0f, 0.0f));
    floorShader.setMat4("model", model);
    floorShader.setMat4("view", view);
    floorShader.setMat4("projection", projection);
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);



    //ERBA REFACTORRRRRRR
    grassShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, transparentTexture);
    grassShader.setMat4("view", view);
    grassShader.setMat4("projection", projection);
    glBindVertexArray(grassVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 2*grassAmount); // 100 triangles of 6 vertices each
    glBindVertexArray(0);



    //TORCIA
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, torciaTexture);
    torciaShader.use();
    view = glm::mat4(1.0f);
    torciaShader.setMat4("view", view);
    torciaShader.setMat4("projection", projection);
    modelMesh = glm::mat4(1.0f);

    modelMesh = glm::scale(modelMesh, glm::vec3(0.1f, 0.1f, 0.1f));
    modelMesh = glm::translate(modelMesh, glm::vec3(-1.2f, -0.6f, -2.0f));
    modelMesh = glm::rotate(modelMesh, (float)glm::radians(30.0), glm::vec3(0.0f, 0.0f, 1.0f));
    modelMesh = glm::rotate(modelMesh, (float)glm::radians(70.0), glm::vec3(0.0f, 1.0f, 0.0f));
    modelMesh = glm::rotate(modelMesh, (float)glm::radians(50.0), glm::vec3(1.0f, 0.0f, 0.0f));
    torciaShader.setMat4("model", modelMesh);
    torciaModel.Draw(torciaShader);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------

  glfwTerminate();
  return 0;
}



