#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader_m.h"
#include "camera.h"
#include "model.h"

#include <iostream>
#include <vector>
#include <string>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = (float)SCR_WIDTH / 2.0;
float lastY = (float)SCR_HEIGHT / 2.0;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// meshes
unsigned int planeVAO;

int main()
{
  // glfw: initialize and configure
  // ------------------------------
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  // glfw window creation
  // --------------------
  GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Slenderman", NULL, NULL);
  if (window == NULL)
  {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetCursorPosCallback(window, mouse_callback);
  glfwSetScrollCallback(window, scroll_callback);
  glfwSetWindowPos(window, 200, 50);

  // tell GLFW to capture our mouse
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
  //stbi_set_flip_vertically_on_load(true);

  // configure global opengl state
  // -----------------------------
  glEnable(GL_DEPTH_TEST);

  // build and compile shaders
  // -------------------------

  Shader slenderShader("model_loading.vs", "model_loading.fs");
  Shader torciaShader("model_loading.vs", "model_loading.fs");
  Shader forestShader("forest.vs", "forest.fs");
  Shader floorShader("model_loading.vs", "model_loading.fs");
  Shader grassShader("grass.vs", "grass.fs");

  // load models
  // -----------
  Model slenderModel("resources/models/Slenderman/Slenderman.obj");
  Model torciaModel("resources/models/Torcia/torcia.dae");
  Model treeModel("resources/models/Tree/oaktrees.obj");


  float planeVertices[] = {
      // positions            // normals         // texcoords
       500.0f, 0.0f,  500.0f,  0.0f, 1.0f, 0.0f,  500.0f,  0.0f,
      -500.0f, 0.0f,  500.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
      -500.0f, 0.0f, -500.0f,  0.0f, 1.0f, 0.0f,   0.0f, 500.0f,

       500.0f, 0.0f,  500.0f,  0.0f, 1.0f, 0.0f,  500.0f,  0.0f,
      -500.0f, 0.0f, -500.0f,  0.0f, 1.0f, 0.0f,   0.0f, 500.0f,
       500.0f, 0.0f, -500.0f,  0.0f, 1.0f, 0.0f,  500.0f, 500.0f
  };
  // plane VAO
  unsigned int planeVBO;
  glGenVertexArrays(1, &planeVAO);
  glGenBuffers(1, &planeVBO);
  glBindVertexArray(planeVAO);
  glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
  glBindVertexArray(0);


  float transparentVertices[] = {
      // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
      0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
      0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
      1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

      0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
      1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
      1.0f,  0.5f,  0.0f,  1.0f,  0.0f
  };

  unsigned int transparentVAO, transparentVBO;
  glGenVertexArrays(1, &transparentVAO);
  glGenBuffers(1, &transparentVBO);
  glBindVertexArray(transparentVAO);
  glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  glBindVertexArray(0);


  
  // load textures
  // -------------

  unsigned int slenderTexture = loadTexture("resources/models/Slenderman/diffuse.png");
  unsigned int torciaTexture = loadTexture("resources/models/Torcia/DefaultMaterial_albedo.jpg");
  unsigned int floorTexture = loadTexture("resources/textures/floor/floor.jpg");
  unsigned int transparentTexture = loadTexture("resources/textures/grass.png");

  vector<glm::vec3> vegetation
  {
      glm::vec3(-5.5f, -3.5f, -5.48f),
      glm::vec3(-5.0f, -3.5f, -5.48f)
  };

  grassShader.use();
  grassShader.setInt("texture1", 0);



  // shader configuration
  // --------------------


  // model configuration
  // --------------------
  int side = 20;
  unsigned int amount = side * side;
  glm::mat4* modelMatrices;
  modelMatrices = new glm::mat4[amount];
  srand(glfwGetTime()); // initialize random seed	
  float offset = 40.0f;
  for (int i = 0; i < side; i++) {

      for (int j = 0; j < side; j++) {
          int index = (i * side) + j;
          glm::mat4 model = glm::mat4(1.0f);
          // 1. translation: displace along circle with 'radius' in range [-offset, offset]
          float x = (i - side/2) * offset;
          float y = -4.0f;
          float z = (j - side/2) * offset;
          model = glm::translate(model, glm::vec3(x, y, z));

          // 2. scale: Scale between 0.05 and 0.25f
          float scale = (rand() % 20) / 100.0f + 0.05;
          model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));

          // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
          float rotAngle = (rand() % 360);
          model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

          // 4. now add to list of matrices
          modelMatrices[index] = model;
          
      }

  }

  unsigned int buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);

  for (unsigned int i = 0; i < treeModel.meshes.size(); i++) {
      unsigned int VAO = treeModel.meshes[i].VAO;
      glBindVertexArray(VAO);
      // set attribute pointers for matrix (4 times vec4)
      glEnableVertexAttribArray(3);
      glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
      glEnableVertexAttribArray(4);
      glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
      glEnableVertexAttribArray(5);
      glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
      glEnableVertexAttribArray(6);
      glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

      glVertexAttribDivisor(3, 1);
      glVertexAttribDivisor(4, 1);
      glVertexAttribDivisor(5, 1);
      glVertexAttribDivisor(6, 1);

      glBindVertexArray(0);
  }


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
    processInput(window);

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
    //modelMesh = glm::translate(modelMesh, glm::vec3(1.0f, 0.0f, -8.0f)); // translate it down so it's at the center of the scene
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
        glDrawElementsInstanced(GL_TRIANGLES, treeModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, amount);
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
    glBindVertexArray(planeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);



    //ERBA REFACTORRRRRRR
    grassShader.use();
    glBindVertexArray(transparentVAO);
    glBindTexture(GL_TEXTURE_2D, transparentTexture);
    grassShader.setMat4("view", view);
    grassShader.setMat4("projection", projection);

    for (unsigned int i = 0; i < vegetation.size(); i++)
    {
        glm::mat4 grassModel = glm::mat4(1.0f);
        grassModel = glm::translate(grassModel, vegetation[i]);
    
        grassShader.setMat4("model", grassModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


    for (unsigned int i = 0; i < vegetation.size(); i++)
    {
        glm::mat4 grassModel = glm::mat4(1.0f);
        glm::vec3 offset = vegetation[i];
        offset.x = offset.x + 0.5f;
        offset.z = offset.z + 0.5f;
        grassModel = glm::translate(grassModel, offset);
        grassModel = glm::rotate(grassModel, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
        grassShader.setMat4("model", grassModel);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

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






    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // optional: de-allocate all resources once they've outlived their purpose:
  // ------------------------------------------------------------------------

  glfwTerminate();
  return 0;
}


// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.ProcessKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.ProcessKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.ProcessKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.ProcessKeyboard(RIGHT, deltaTime);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
  // make sure the viewport matches the new window dimensions; note that width and 
  // height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
  if (firstMouse)
  {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xoffset = xpos - lastX;
  float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

  lastX = xpos;
  lastY = ypos;

  camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
  unsigned int textureID;
  glGenTextures(1, &textureID);

  int width, height, nrComponents;
  unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data)
  {
    GLenum format;
    if (nrComponents == 1)
      format = GL_RED;
    else if (nrComponents == 3)
      format = GL_RGB;
    else if (nrComponents == 4)
      format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  }
  else
  {
    std::cout << "Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}
