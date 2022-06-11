#pragma once
#include "shader_m.h"
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>

void renderFloor(
    Shader& floorShader,
    unsigned int& floorTexture,
    unsigned int& floorVAO,
    glm::mat4& view,
    glm::mat4& projection
) {
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
}

void renderForest(
    Shader& forestShader,
    Model& treeModel,
    glm::mat4& view,
    glm::mat4& projection
) {
    unsigned int treeAmount = TREE_QUAD_SIDE * TREE_QUAD_SIDE;
    forestShader.use();
    forestShader.setMat4("projection", projection);
    forestShader.setMat4("view", view);

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
}

void renderGrass(
    Shader& grassShader,
    unsigned int& grassTexture,
    unsigned int& grassVAO,
    glm::mat4& view,
    glm::mat4& projection
) {
    unsigned int grassAmount = GRASS_QUAD_SIDE * GRASS_QUAD_SIDE;
    grassShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    grassShader.setMat4("view", view);
    grassShader.setMat4("projection", projection);
    glBindVertexArray(grassVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 2 * grassAmount);
    glBindVertexArray(0);
}

void renderSlenderman(
    Shader& slenderShader, 
    unsigned int& slenderTexture, 
    Model & slenderModel, 
    glm::vec3& translationMatrix,
    glm::mat4& view,
    glm::mat4& projection
) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, slenderTexture);
    slenderShader.use();
    slenderShader.setMat4("view", view);
    slenderShader.setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, translationMatrix);
    model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));
    slenderShader.setMat4("model", model);
    slenderModel.Draw(slenderShader);
}

void renderFlashlight(
    Shader& flashlightShader,
    unsigned int& flashlightTexture,
    Model& flashlightModel,
    glm::mat4& view,
    glm::mat4& projection
) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, flashlightTexture);
    flashlightShader.use();
    view = glm::mat4(1.0f);
    flashlightShader.setMat4("view", view);
    flashlightShader.setMat4("projection", projection);
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    model = glm::translate(model, glm::vec3(-1.2f, -0.6f, -2.0f));
    model = glm::rotate(model, (float)glm::radians(30.0), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, (float)glm::radians(70.0), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, (float)glm::radians(50.0), glm::vec3(1.0f, 0.0f, 0.0f));
    flashlightShader.setMat4("model", model);
    flashlightModel.Draw(flashlightShader);
}

