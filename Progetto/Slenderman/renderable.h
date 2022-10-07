#pragma once

#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>

#include "camera.h"
#include "light_utils.h"
#include "model.h";
#include "shader_m.h";

class Renderable {
protected:
    Shader* _shader;
    unsigned int _texture;

public:
    virtual void render(const Camera& camera, const LightUtils& lightUtils) = 0;
};

class VAORenderable : public Renderable {
protected:
    unsigned int _VAO;
    glm::mat4 _transform;

    static unsigned int _initRectVAO(const float dimension);
};

class ModelRenderable : public Renderable {
protected:
    Model* _model;
    glm::mat4 _transform;
};

class InstancedModelRenderable : public Renderable {
protected:
    Model* _model;
    std::vector<glm::mat4> _transforms;

    void _initUsingDynamicMapAlgorithm(const int quadSide, const int vaoObjectSide, const float offset, const glm::vec3& scaleMatrix, const bool useRandomOffset);
};

unsigned int VAORenderable::_initRectVAO(const float dimension) {
    float rectVertices[] = {
        // positions            // normals         // texcoords
         dimension, 0.0f,  dimension,  0.0f, 1.0f, 0.0f,  dimension,  0.0f,
        -dimension, 0.0f,  dimension,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
        -dimension, 0.0f, -dimension,  0.0f, 1.0f, 0.0f,   0.0f, dimension,

         dimension, 0.0f,  dimension,  0.0f, 1.0f, 0.0f,  dimension,  0.0f,
        -dimension, 0.0f, -dimension,  0.0f, 1.0f, 0.0f,   0.0f, dimension,
         dimension, 0.0f, -dimension,  0.0f, 1.0f, 0.0f,  dimension, dimension
    };

    unsigned int rectVAO;
    unsigned int rectVBO;

    glGenVertexArrays(1, &rectVAO);
    glGenBuffers(1, &rectVBO);

    glBindVertexArray(rectVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectVertices), rectVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
    return rectVAO;
}

void InstancedModelRenderable::_initUsingDynamicMapAlgorithm(const int quadSide, const int vaoObjectSide, const float offset, const glm::vec3& scaleMatrix, const bool useRandomOffset) {
    int numVAO = (quadSide / vaoObjectSide) * (quadSide / vaoObjectSide);
    unsigned int amount = quadSide * quadSide;
    vector<glm::mat4*> _transformMatrix;
    for (int k = 0; k < numVAO; k++)
        _transformMatrix.push_back(new glm::mat4[(amount / numVAO)]);

    srand(glfwGetTime());

    for (int i = 0; i < quadSide; i++) {
        for (int j = 0; j < quadSide; j++) {
            glm::mat4 transform = glm::mat4(1.0f);

            float rx = 0.0f;
            float rz = 0.0f;
            if (useRandomOffset) {
                rx = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * offset / 2;
                rz = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * offset / 2;
            }

            // 1. Traslazione degli alberi in base all'indice della griglia
            float x = (i - quadSide / 2) * offset + rx;
            float y = -4.0f;
            float z = (j - quadSide / 2) * offset + rz;
            transform = glm::translate(transform, glm::vec3(x, y, z));
            // 2. Scala gli alberi
            transform = glm::scale(transform, scaleMatrix);
            // 3. Rotazione randomica
            float rotAngle = (rand() % 360);
            transform = glm::rotate(transform, rotAngle, glm::vec3(0.0f, 1.0f, 0.0f));

            // 4. Aggiunge al matrice al modello
            unsigned int vaoI = floor(i / vaoObjectSide);
            unsigned int vaoJ = floor(j / vaoObjectSide);
            unsigned int vaoIndex = (vaoI * (quadSide / vaoObjectSide)) + vaoJ;

            unsigned int matrixIndex = ((i % vaoObjectSide) * vaoObjectSide) + (j % vaoObjectSide);
            _transformMatrix[vaoIndex][matrixIndex] = transform;
            _transforms.push_back(transform);
        }
    }

    for (int k = 0; k < numVAO; k++) {
        glm::mat4* transform = _transformMatrix[k];

        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);

        glBufferData(GL_ARRAY_BUFFER, (amount / numVAO) * sizeof(glm::mat4), &transform[0], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < _model->meshes.size(); i++) {
            unsigned int VAO;
            glGenVertexArrays(1, &VAO);
            glBindVertexArray(VAO);

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

            _model->meshes[i].VAOs.push_back(VAO);
            glBindVertexArray(0);

        }
    }

    for (unsigned int i = 0; i < _model->meshes.size(); i++) {
        _model->meshes[i].setupVAOs();
    }
}