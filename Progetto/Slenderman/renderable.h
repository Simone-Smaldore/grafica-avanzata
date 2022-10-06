#pragma once

#include <glm/glm.hpp>

#include "camera.h"
#include "light_utils.h"
#include "model.h";
#include "shader_m.h";

class Renderable {
protected:
    Shader* _shader;
    unsigned int _texture;
    glm::mat4 _transform;

public:
    virtual void render(const Camera& camera, const LightUtils& lightUtils) = 0;
};

class VAORenderable : public Renderable {
protected:
    unsigned int _VAO;

    static unsigned int _initRectVAO(const float dimension);
};

class ModelRenderable : public Renderable {
protected:
    Model* _model;
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