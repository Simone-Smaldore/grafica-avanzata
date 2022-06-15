#pragma once
#include "shader_m.h"
#include "constants.h"
#include <glm/gtc/matrix_transform.hpp>
#include "render_text.h"
#include <sstream>
#include <string>


//TODO Transformare in oggetto renderer

vector<int> getVaoIndexesFromCamera(Camera& camera, float offset, int quadSide, int vaoObjectSide);

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
    Camera& camera
) {
    forestShader.use();
    forestShader.setMat4("projection", projection);
    forestShader.setMat4("view", view);

    //TODO: Implementare strategia per scartare alcuni k in modo da non intersecare le fence ?
    vector<int> VAO_indexes = getVaoIndexesFromCamera(camera, TREE_OFFSET, TREE_QUAD_SIDE, VAO_OBJECTS_SIDE_FOREST);

    unsigned int num_VAO = (TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_FOREST) * (TREE_QUAD_SIDE / VAO_OBJECTS_SIDE_FOREST);
    unsigned int num_element_for_VAO = (TREE_QUAD_SIDE * TREE_QUAD_SIDE) / num_VAO;

    for (unsigned int k = 0; k < VAO_indexes.size(); k++) {
        for (unsigned int i = 0; i < treeModel.meshes.size(); i++) {
            for (unsigned int j = 0; j < treeModel.meshes[i].textures.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                string name = treeModel.meshes[i].textures[j].type;
                glUniform1i(glGetUniformLocation(forestShader.ID, (name + std::to_string(j + 1)).c_str()), j);
                glBindTexture(GL_TEXTURE_2D, treeModel.meshes[i].textures[j].id);
            }
            int vao_index = max(VAO_indexes[k], 0);
            glBindVertexArray(treeModel.meshes[i].VAOs[vao_index]);
            glDrawElementsInstanced(GL_TRIANGLES, treeModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, num_element_for_VAO);
            glBindVertexArray(0);
        }
    }

}

void renderFence(
    Shader& fenceShader,
    unsigned int& fenceTexture,
    Model& fenceModel,
    glm::mat4& view,
    glm::mat4& projection
) {
    fenceShader.use();
    fenceShader.setMat4("projection", projection);
    fenceShader.setMat4("view", view);
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
    grassShader.use();
    grassShader.setMat4("projection", projection);
    grassShader.setMat4("view", view);

    vector<int> VAO_indexes = getVaoIndexesFromCamera(camera, GRASS_OFFSET, GRASS_QUAD_SIDE, VAO_OBJECTS_SIDE_GRASS);

    unsigned int num_VAO = (GRASS_QUAD_SIDE / VAO_OBJECTS_SIDE_GRASS) * (GRASS_QUAD_SIDE / VAO_OBJECTS_SIDE_GRASS);
    unsigned int num_element_for_VAO = (GRASS_QUAD_SIDE * GRASS_QUAD_SIDE) / num_VAO;

    for (unsigned int k = 0; k < VAO_indexes.size(); k++) {
        for (unsigned int i = 0; i < grassModel.meshes.size(); i++) {
            for (unsigned int j = 0; j < grassModel.meshes[i].textures.size(); j++) {
                glActiveTexture(GL_TEXTURE0 + j);
                string name = grassModel.meshes[i].textures[j].type;
                glUniform1i(glGetUniformLocation(grassShader.ID, (name + std::to_string(j + 1)).c_str()), j);
                glBindTexture(GL_TEXTURE_2D, grassModel.meshes[i].textures[j].id);
            }
            int vao_index = max(VAO_indexes[k], 0);
            glBindVertexArray(grassModel.meshes[i].VAOs[vao_index]);
            glDrawElementsInstanced(GL_TRIANGLES, grassModel.meshes[i].indices.size(), GL_UNSIGNED_INT, 0, num_element_for_VAO);
            glBindVertexArray(0);
        }
    }

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

void renderInfo(Camera& camera) {
    std::stringstream ssx; 
    ssx << "x: " << camera.Position.x;
    std::string x = ssx.str();
    RenderText(x, 1100.0f, 250.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    std::stringstream ssz;
    ssz << "z: " << camera.Position.z;
    std::string z = ssz.str();
    RenderText(z, 1100.0f, 200.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
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
    int vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    x_index++;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    z_index++;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    x_index--;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    x_index--;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    z_index--;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    z_index--;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    x_index++;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    x_index++;
    vao_index = (x_index * (quadSide / vaoObjectSide)) + z_index;
    result.push_back(vao_index);

    return result;
}
    

