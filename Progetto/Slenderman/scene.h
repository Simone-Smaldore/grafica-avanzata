#pragma once
#include "constants.h"

void initFloor(unsigned int& floorVAO);
void initTreeForest(Model& treeModel);
void initGrass(unsigned int& grassVAO);

void initScene(
	unsigned int& floorVAO,
	Model& treeModel,
    unsigned int& grassVAO
) {
	initFloor(floorVAO);
	initTreeForest(treeModel);
    initGrass(grassVAO);
}

void initFloor(unsigned int& floorVAO) {
	float floorVertices[] = {
		// positions            // normals         // texcoords
		 500.0f, 0.0f,  500.0f,  0.0f, 1.0f, 0.0f,  500.0f,  0.0f,
		-500.0f, 0.0f,  500.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-500.0f, 0.0f, -500.0f,  0.0f, 1.0f, 0.0f,   0.0f, 500.0f,

		 500.0f, 0.0f,  500.0f,  0.0f, 1.0f, 0.0f,  500.0f,  0.0f,
		-500.0f, 0.0f, -500.0f,  0.0f, 1.0f, 0.0f,   0.0f, 500.0f,
		 500.0f, 0.0f, -500.0f,  0.0f, 1.0f, 0.0f,  500.0f, 500.0f
	};

	unsigned int floorVBO;
	glGenVertexArrays(1, &floorVAO);
	glGenBuffers(1, &floorVBO);
	glBindVertexArray(floorVAO);
	glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);
}

void initTreeForest(Model& treeModel) {
    unsigned int amount = TREE_QUAD_SIDE * TREE_QUAD_SIDE;
    glm::mat4* modelMatrices;
    modelMatrices = new glm::mat4[amount];
    srand(glfwGetTime()); // initialize random seed	
    float offset = 40.0f;
    for (int i = 0; i < TREE_QUAD_SIDE; i++) {
        for (int j = 0; j < TREE_QUAD_SIDE; j++) {
            int index = (i * TREE_QUAD_SIDE) + j;
            glm::mat4 model = glm::mat4(1.0f);
            // 1. Traslazione degli alberi in base all'indice della griglia
            float x = (i - TREE_QUAD_SIDE / 2) * offset;
            float y = -4.0f;
            float z = (j - TREE_QUAD_SIDE / 2) * offset;
            model = glm::translate(model, glm::vec3(x, y, z));
            // 2. Scala gli alberi
            model = glm::scale(model, glm::vec3(0.08f, 0.08f, 0.08f));
            // 3. Rotazione randomica
            float rotAngle = (rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            // 4. Aggiunge al matrice al modello
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
}

void initGrass(unsigned int& grassVAO) {
    unsigned int grassAmount = GRASS_QUAD_SIDE * GRASS_QUAD_SIDE;
    glm::mat4* grassModelMatrices;
    grassModelMatrices = new glm::mat4[2 * grassAmount];
    srand(glfwGetTime()); // initialize random seed	
    float grassOffset = 1.5f;
    for (int i = 0; i < GRASS_QUAD_SIDE; i++) {
        for (int j = 0; j < GRASS_QUAD_SIDE; j++) {
            int index = (i * GRASS_QUAD_SIDE) + j;
            glm::mat4 model = glm::mat4(1.0f);
            // 1. Traslazione dell'erba in base all'indice
            float rx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 1.5;
            float rz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 1.5;
            float x = (i - GRASS_QUAD_SIDE / 2) * grassOffset + rx;
            float y = -3.5f;
            float z = (j - GRASS_QUAD_SIDE / 2) * grassOffset + rz;
            model = glm::translate(model, glm::vec3(x, y, z));
            grassModelMatrices[index] = model;
            // 2. Aggiungiamo anche il quadrato perpendicolare
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x + 0.5f, y, z + 0.5f));
            model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
            grassModelMatrices[grassAmount + index] = model;
        }
    }


    float grassVertices[] = {
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };

    unsigned int grassVBO;
    glGenVertexArrays(1, &grassVAO);
    glGenBuffers(1, &grassVBO);
    glBindVertexArray(grassVAO);
    glBindBuffer(GL_ARRAY_BUFFER, grassVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(grassVertices), grassVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    unsigned int instanceVBO;
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, 2 * grassAmount * sizeof(glm::mat4), &grassModelMatrices[0], GL_STATIC_DRAW);
    // also set instance data
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