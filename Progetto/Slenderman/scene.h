#pragma once
#include "constants.h"


void initFloor(unsigned int& floorVAO);
void initTreeForest(Model& treeModel);
void initFence(Model& fenceModel);
void initGrass(Model& grassModel);

void initScene(
	unsigned int& floorVAO,
	Model& treeModel,
    Model& fenceModel,
    Model& grassModel
) {
	initFloor(floorVAO);
	initTreeForest(treeModel);
    initFence(fenceModel);
    initGrass(grassModel);
}

void initFloor(unsigned int& floorVAO) {
    float floorVertices[] = {
        // positions            // normals         // texcoords
         2000.0f, 0.0f,  2000.0f,  0.0f, 1.0f, 0.0f,  2000.0f,  0.0f,
        -2000.0f, 0.0f,  2000.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -2000.0f, 0.0f, -2000.0f,  0.0f, 1.0f, 0.0f,   0.0f, 2000.0f,

         2000.0f, 0.0f,  2000.0f,  0.0f, 1.0f, 0.0f,  2000.0f,  0.0f,
        -2000.0f, 0.0f, -2000.0f,  0.0f, 1.0f, 0.0f,   0.0f, 2000.0f,
         2000.0f, 0.0f, -2000.0f,  0.0f, 1.0f, 0.0f,  2000.0f, 2000.0f
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
    int num_VAO = (TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_FOREST) * (TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_FOREST);

    unsigned int amount = TREE_QUAD_SIDE * TREE_QUAD_SIDE;
    vector<glm::mat4*> modelMatrices;
    for (int k = 0; k < num_VAO; k++) {
        modelMatrices.push_back(new glm::mat4[(amount / num_VAO)]);
    }
    srand(glfwGetTime()); // initialize random seed	
    float offset = TREE_OFFSET;
    for (int i = 0; i < TREE_QUAD_SIDE; i++) {
        for (int j = 0; j < TREE_QUAD_SIDE; j++) {
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
            unsigned int vao_i = floor(i / VAO_OBJECTS_SIDE_FOREST);
            unsigned int vao_j = floor(j / VAO_OBJECTS_SIDE_FOREST);
            unsigned int vao_index = (vao_i * (TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_FOREST)) + vao_j;
            unsigned int matrix_index = ((i% VAO_OBJECTS_SIDE_FOREST) * VAO_OBJECTS_SIDE_FOREST) +  (j % VAO_OBJECTS_SIDE_FOREST);
            //cout <<"vao i: " << vao_i << " vao_j: " << vao_j << " vao_index: " << vao_index << "matrix_index:" << matrix_index << endl;
            modelMatrices[vao_index][matrix_index] = model;
        }
    }

    for (int k = 0; k < num_VAO; k++) {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glm::mat4* model = modelMatrices[k];
        glBufferData(GL_ARRAY_BUFFER, (amount/num_VAO) * sizeof(glm::mat4), &model[0], GL_STATIC_DRAW);
        for (unsigned int i = 0; i < treeModel.meshes.size(); i++) {
            unsigned int VAO;
            glGenVertexArrays(1, &VAO);
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

            treeModel.meshes[i].VAOs.push_back(VAO);
            glBindVertexArray(0);
            
        }
    }
    
    for (unsigned int i = 0; i < treeModel.meshes.size(); i++) {
        treeModel.meshes[i].setupVAOs();
    }
   
}

void initFence(Model& fenceModel) {
    int sides = 4;
    int num_VAO = NUM_FENCES_FOR_SIDE * sides;
    glm::mat4* modelMatrices = new glm::mat4[num_VAO];
    srand(glfwGetTime()); // initialize random seed	
    float offset = FENCE_OFFSET;
    float center_distance = (NUM_FENCES_FOR_SIDE / 2) * FENCE_OFFSET;
    for (int i = 0; i < NUM_FENCES_FOR_SIDE; i++) {
        //FRONT
        glm::mat4 model = glm::mat4(1.0f);
        float x = (i - NUM_FENCES_FOR_SIDE / 2) * offset;
        float y = -5.0f;
        float z = -center_distance;
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrices[i] = model;    

        //LEFT
        model = glm::mat4(1.0f);
        x = -center_distance -9.0f;
        z = ((i - NUM_FENCES_FOR_SIDE / 2 )) * offset + 10.0f;
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        modelMatrices[NUM_FENCES_FOR_SIDE + i] = model;

        //RIGHT
        model = glm::mat4(1.0f);
        x = center_distance - 9.0f;
        z = ((i - NUM_FENCES_FOR_SIDE / 2)) * offset +10.0f;
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        modelMatrices[(2*NUM_FENCES_FOR_SIDE + i)] = model;

        //BACK
        model = glm::mat4(1.0f); 
        x = (i - NUM_FENCES_FOR_SIDE / 2) * offset;
        z = center_distance -1.0f;
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
        model = glm::rotate(model, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
        modelMatrices[(3 * NUM_FENCES_FOR_SIDE + i)] = model;
    }

    for (int k = 0; k < num_VAO; k++) {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &modelMatrices[k], GL_STATIC_DRAW);
        for (unsigned int i = 0; i < fenceModel.meshes.size(); i++) {
            unsigned int VAO;
            glGenVertexArrays(1, &VAO);
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

            fenceModel.meshes[i].VAOs.push_back(VAO);
            glBindVertexArray(0);

        }
    }

    for (unsigned int i = 0; i < fenceModel.meshes.size(); i++) {
        fenceModel.meshes[i].setupVAOs();
    }
}

void initGrass(Model& grassModel) {
    int num_VAO = (GRASS_QUAD_SIDE / VAO_OBJECTS_SIDE_GRASS) * (GRASS_QUAD_SIDE / VAO_OBJECTS_SIDE_GRASS);

    unsigned int amount = GRASS_QUAD_SIDE * GRASS_QUAD_SIDE;
    vector<glm::mat4*> modelMatrices;
    for (int k = 0; k < num_VAO; k++) {
        modelMatrices.push_back(new glm::mat4[(amount / num_VAO)]);
    }
    srand(glfwGetTime()); // initialize random seed	
    float offset = GRASS_OFFSET;
    for (int i = 0; i < GRASS_QUAD_SIDE; i++) {
        for (int j = 0; j < GRASS_QUAD_SIDE; j++) {
            glm::mat4 model = glm::mat4(1.0f);
            float rx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * GRASS_OFFSET/2;
            float rz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * GRASS_OFFSET/2;
            // 1. Traslazione degli alberi in base all'indice della griglia
            float x = (i - GRASS_QUAD_SIDE / 2) * offset + rx;
            float y = -4.0f;
            float z = (j - GRASS_QUAD_SIDE / 2) * offset + rz;
            model = glm::translate(model, glm::vec3(x, y, z));
            // 2. Scala gli alberi
            model = glm::scale(model, glm::vec3(0.015f, 0.01f, 0.015f));
            // 3. Rotazione randomica
            float rotAngle = (rand() % 360);
            model = glm::rotate(model, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));
            
            // 4. Aggiunge al matrice al modello
            unsigned int vao_i = floor(i / VAO_OBJECTS_SIDE_GRASS);
            unsigned int vao_j = floor(j / VAO_OBJECTS_SIDE_GRASS);
            unsigned int vao_index = (vao_i * (GRASS_QUAD_SIDE / VAO_OBJECTS_SIDE_GRASS)) + vao_j;
            unsigned int matrix_index = ((i % VAO_OBJECTS_SIDE_GRASS) * VAO_OBJECTS_SIDE_GRASS) + (j % VAO_OBJECTS_SIDE_GRASS);
            //cout <<"vao i: " << vao_i << " vao_j: " << vao_j << " vao_index: " << vao_index << "matrix_index:" << matrix_index << endl;
            modelMatrices[vao_index][matrix_index] = model;
        }
    }

    for (int k = 0; k < num_VAO; k++) {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glm::mat4* model = modelMatrices[k];
        glBufferData(GL_ARRAY_BUFFER, (amount / num_VAO) * sizeof(glm::mat4), &model[0], GL_STATIC_DRAW);
        for (unsigned int i = 0; i < grassModel.meshes.size(); i++) {
            unsigned int VAO;
            glGenVertexArrays(1, &VAO);
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

            grassModel.meshes[i].VAOs.push_back(VAO);
            glBindVertexArray(0);

        }
    }

    for (unsigned int i = 0; i < grassModel.meshes.size(); i++) {
        grassModel.meshes[i].setupVAOs();
    }

}
