#pragma once
#include "shader_m.h"
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include "render_text.h"
#include <sstream>
#include <string>

//TODO Transformare in oggetto renderer con metodi privati

//Privati
void renderDynamicMap(Shader& shader, Model& modelObj, vector<int>& VAO_indexes, int quadSide, int vaoObjectSide);
vector<int> getVaoIndexesFromCamera(Camera& camera, float offset, int quadSide, int vaoObjectSide);
void initLightShader(Shader& shader);

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
    glm::mat4& projection,
    Camera& camera,
    vector<int>& positionsPointOfinterest
) {

    initLightShader(forestShader);
    forestShader.use();
    forestShader.setMat4("projection", projection);
    forestShader.setMat4("view", view);
    forestShader.setFloat("alphaValue", 0.7f);
    //TODO: Implementare strategia per scartare alcuni k in modo da non renderizzare pezzi di foresta ?

    vector<int> VAO_indexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_FOREST);
    for (int i = 0; i < positionsPointOfinterest.size(); i++) {
        int val = positionsPointOfinterest[i];
        auto it = find(VAO_indexes.begin(), VAO_indexes.end(), val);
        if (it != VAO_indexes.end()){
            int deleteIndex = it - VAO_indexes.begin();
            VAO_indexes.erase(VAO_indexes.begin() + deleteIndex);
        }
    }

    renderDynamicMap(forestShader, treeModel, VAO_indexes, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_FOREST);
}

void renderFence(
    Shader& fenceShader,
    unsigned int& fenceTexture,
    Model& fenceModel,
    glm::mat4& view,
    glm::mat4& projection
) {
    initLightShader(fenceShader);
    fenceShader.use();
    fenceShader.setMat4("projection", projection);
    fenceShader.setMat4("view", view);
    fenceShader.setFloat("alphaValue", 0.7f);
    int num_VAO = NUM_FENCES_FOR_SIDE * 4;
    //TODO: Renderizzare solo un sottoinsieme delle fence come per la foresta
    for (int k = 0; k < num_VAO; k++) {
        for (int i = 0; i < fenceModel.meshes.size(); i++) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, fenceTexture);
            glBindVertexArray(fenceModel.meshes[i].VAOs[k]);
            glDrawElements(GL_TRIANGLES, fenceModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }
    }
}

void renderGrass(
    Shader& grassShader,
    Model& grassModel,
    glm::mat4& view,
    glm::mat4& projection,
    Camera& camera
) {
    initLightShader(grassShader);
    grassShader.use();
    grassShader.setMat4("projection", projection);
    grassShader.setMat4("view", view);
    grassShader.setFloat("alphaValue", 0.1f);

    vector<int> VAO_indexes = getVaoIndexesFromCamera(camera, GRASS_OFFSET, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);
    renderDynamicMap(grassShader, grassModel, VAO_indexes, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);
}


void renderSlenderman(
    Shader& slenderShader,
    unsigned int& slenderTexture,
    Model& slenderModel,
    glm::vec3& translationMatrix,
    glm::mat4& view,
    glm::mat4& projection
) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, slenderTexture);
    initLightShader(slenderShader);
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
    initLightShader(flashlightShader);
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

void renderInfo(Camera& camera, int fps) {
    std::stringstream ssx; 
    ssx << "x: " << camera.Position.x;
    std::string x = ssx.str();
    RenderText(x, 1100.0f, 250.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    std::stringstream ssz;
    ssz << "z: " << camera.Position.z;
    std::string z = ssz.str();
    RenderText(z, 1100.0f, 200.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    vector<int> indexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_FOREST);
    int k_index = indexes[(indexes.size() - 1) /2];
    std::stringstream ssk;
    ssk << "k_index: " << k_index;
    std::string k = ssk.str();
    RenderText(k, 1100.0f, 150.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

    std::stringstream ssfps;
    ssfps << "fps: " << fps;
    std::string fps_str = ssfps.str();
    RenderText(fps_str, 1100.0f, 1000.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
}

void renderDynamicMap(Shader& shader, Model& modelObj, vector<int>& VAO_indexes, int quadSide, int vaoObjectSide)  {
    unsigned int num_VAO = (quadSide / vaoObjectSide) * (quadSide / vaoObjectSide);
    unsigned int num_element_for_VAO = (quadSide * quadSide) / num_VAO;

    for (unsigned int k = 0; k < VAO_indexes.size(); k++) {
        for (unsigned int i = 0; i < modelObj.meshes.size(); i++) {
            for (unsigned int j = 0; j < modelObj.meshes[i].textures.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                string name = modelObj.meshes[i].textures[j].type;
                glUniform1i(glGetUniformLocation(shader.ID, (name + std::to_string(j + 1)).c_str()), j);
                glBindTexture(GL_TEXTURE_2D, modelObj.meshes[i].textures[j].id);
            }
            int vao_index = max(VAO_indexes[k], 0);
            glBindVertexArray(modelObj.meshes[i].VAOs[vao_index]);
            glDrawElementsInstanced(GL_TRIANGLES, modelObj.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, num_element_for_VAO);
            glBindVertexArray(0);
        }
    }
}

vector<int> getVaoIndexesFromCamera(Camera& camera, float offset, int quadSide, int vaoObjectSide) {
    vector<int> result;

    float x_camera = camera.Position.x + (offset * quadSide / 2) + offset / 2;
    if (x_camera < 0.0f) {
        x_camera = 0.0f;
    }
    if (x_camera >= offset * quadSide) {
        x_camera = offset * quadSide - 0.1f;
    }

    float z_camera = camera.Position.z + (offset * quadSide / 2) + offset / 2;
    if (z_camera < 0.0f) {
        z_camera = 0.0f;
    }
    if (z_camera >= offset * quadSide) {
        z_camera = offset * quadSide - 0.1f;
    }

    int x_index = floor(x_camera / (vaoObjectSide * offset));
    int z_index = floor(z_camera / (vaoObjectSide * offset));


    for (int i = (x_index - INT_OFFSET_VAO_INDEXES); i <= (x_index + INT_OFFSET_VAO_INDEXES); i++) {
        for (int j = (z_index - INT_OFFSET_VAO_INDEXES); j <= (z_index + INT_OFFSET_VAO_INDEXES); j++) {
            int vao_index = (i * (quadSide / vaoObjectSide)) + j;
            result.push_back(vao_index);
        }
    }

    //cout << "Len List: " << result.size() << endl;

    return result;
}

void initLightShader(Shader& shader) {
    shader.use();
    shader.setVec3("light.position", camera.Position);
    shader.setVec3("light.direction", camera.Front);
    shader.setFloat("light.cutOff", glm::cos(glm::radians(12.5f)));
    shader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
    shader.setVec3("viewPos", camera.Position);

    // light properties
    shader.setVec3("light.ambient", 0.1f, 0.1f, 0.1f);
    // we configure the diffuse intensity slightly higher; the right lighting conditions differ with each lighting method and environment.
    // each environment and lighting type requires some tweaking to get the best out of your environment.
    shader.setVec3("light.diffuse", 0.8f, 0.8f, 0.8f);
    shader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
    shader.setFloat("light.constant", 1.0f);
    shader.setFloat("light.linear", 0.09f);
    shader.setFloat("light.quadratic", 0.032f);

    // material properties
    shader.setFloat("material.shininess", 32.0f);
}
    

