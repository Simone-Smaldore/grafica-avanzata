#pragma once

#include "GLFW/glfw3.h"

#include "light_utils.h"
#include "model_cache.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

class Fence : public InstancedModelRenderable {
public:
    Fence();

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

Fence::Fence() {
    _model = ModelCache::getInstance().findModel(EModel::fence);
    _texture = TextureCache::getInstance().findTexture(ETexture::fence);
    _shader = ShaderCache::getInstance().findShader(EShader::fence);

    srand(glfwGetTime());
    float offset = FENCE_OFFSET;
    float center_distance = (NUM_FENCES_FOR_SIDE / 2) * FENCE_OFFSET;

    int sides = 4;
    int numVAO = NUM_FENCES_FOR_SIDE * sides;

    for (int i = 0; i < NUM_FENCES_FOR_SIDE; i++) {
        //FRONT
        glm::mat4 transform = glm::mat4(1.0f);
        float x = (i - NUM_FENCES_FOR_SIDE / 2) * offset;
        float y = -5.0f;
        float z = -center_distance;
        transform = glm::translate(transform, glm::vec3(x, y, z));
        transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
        transform = glm::rotate(transform, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
        _transforms.push_back(transform);

        //LEFT
        transform = glm::mat4(1.0f);
        x = -center_distance - 9.0f;
        z = ((i - NUM_FENCES_FOR_SIDE / 2)) * offset + 10.0f;
        transform = glm::translate(transform, glm::vec3(x, y, z));
        transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
        _transforms.push_back(transform);

        //RIGHT
        transform = glm::mat4(1.0f);
        x = center_distance - 9.0f;
        z = ((i - NUM_FENCES_FOR_SIDE / 2)) * offset + 10.0f;
        transform = glm::translate(transform, glm::vec3(x, y, z));
        transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
        _transforms.push_back(transform);

        //BACK
        transform = glm::mat4(1.0f);
        x = (i - NUM_FENCES_FOR_SIDE / 2) * offset;
        z = center_distance - 1.0f;
        transform = glm::translate(transform, glm::vec3(x, y, z));
        transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
        transform = glm::rotate(transform, (float)glm::radians(90.0), glm::vec3(0.0f, 1.0f, 0.0f));
        _transforms.push_back(transform);
    }

    for (int k = 0; k < numVAO; k++) {
        unsigned int buffer;
        glGenBuffers(1, &buffer);
        glBindBuffer(GL_ARRAY_BUFFER, buffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), &_transforms[k], GL_STATIC_DRAW);

        for (unsigned int i = 0; i < _model->meshes.size(); i++) {
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

            _model->meshes[i].VAOs.push_back(VAO);
            glBindVertexArray(0);
        }
    }

    for (unsigned int i = 0; i < _model->meshes.size(); i++) {
        _model->meshes[i].setupVAOs();
    }
}

void Fence::render(const Camera& camera, const LightUtils& lightUtils) {
    lightUtils.initLightShader(_shader, camera);
    _shader->use();
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setFloat("alphaValue", 0.7f);

    int num_VAO = NUM_FENCES_FOR_SIDE * 4;
    for (int k = 0; k < num_VAO; k++) {
        for (int i = 0; i < _model->meshes.size(); i++) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, _texture);
            glBindVertexArray(_model->meshes[i].VAOs[k]);
            glDrawElements(GL_TRIANGLES, _model->meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}