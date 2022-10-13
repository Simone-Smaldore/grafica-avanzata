#pragma once

#include <map>
#include <vector>

#include "glm/glm.hpp"

#include "constants.h"
#include "renderable.h"
#include "texture_cache.h"
#include "shader_cache.h"

class Minimap : public VAORenderable {
private:
    unsigned int _framebuffer;
    unsigned int _textureColorBuffer;
    unsigned int _minimapWoodVAO;
    unsigned int _circleVAO;
    unsigned int _personVAO;
    Shader* _minimapWoodShader;
    Shader* _minimapCircleShader;

    std::vector<glm::mat4> _circleTransforms;

    void _initMinimap();
    void _initMinimapMarkers();
    void _buildMinimap(const Camera& camera);

public:
    Minimap(const std::map<int, glm::vec3>& poiInfo);

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

Minimap::Minimap(const std::map<int, glm::vec3>& poiInfo) {
    for (auto poi : poiInfo) {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(-poi.second.x / MAX_W_QUAD_MAP, poi.second.z / MAX_W_QUAD_MAP, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.03f, 0.03f, 0.03f));
        _circleTransforms.push_back(transform);
    }

    _texture = TextureCache::getInstance().findTexture(ETexture::minimap);

    _shader = ShaderCache::getInstance().findShader(EShader::minimap);
    _minimapWoodShader = ShaderCache::getInstance().findShader(EShader::minimapWood);
    _minimapCircleShader = ShaderCache::getInstance().findShader(EShader::minimapCircle);

    _initMinimap();
    _initMinimapMarkers();
}

void Minimap::_initMinimap() {
    float minimapVertices[] = {
        // positions                                                                                           // texCoords
           1.0f - MAP_W_PROP_DIMENSION - MAP_W_PROP_OFFSET, -(1.0f - MAP_H_PROP_DIMENSION - MAP_H_PROP_OFFSET),  1.0f, 0.0f,
           1.0f - MAP_W_PROP_DIMENSION - MAP_W_PROP_OFFSET, -1.0 + MAP_H_PROP_OFFSET                          ,  1.0f, 1.0f,
           1.0f - MAP_W_PROP_OFFSET                       , -1.0 + MAP_H_PROP_OFFSET                          ,  0.0f, 1.0f,

           1.0f - MAP_W_PROP_DIMENSION - MAP_W_PROP_OFFSET, -(1.0f - MAP_H_PROP_DIMENSION - MAP_H_PROP_OFFSET),  1.0f, 0.0f,
           1.0f - MAP_W_PROP_OFFSET                       , -1.0 + MAP_H_PROP_OFFSET                          ,  0.0f, 1.0f,
           1.0f - MAP_W_PROP_OFFSET                       , -(1.0f - MAP_H_PROP_DIMENSION - MAP_H_PROP_OFFSET),  0.0f, 0.0f
    };

    unsigned int minimapVAO;
    unsigned int minimapVBO;
    glGenVertexArrays(1, &minimapVAO);
    glGenBuffers(1, &minimapVBO);
    glBindVertexArray(minimapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, minimapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(minimapVertices), &minimapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


    // framebuffer
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

    // create a color attachment texture
    unsigned int textureColorBuffer;
    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    // create a renderbuffer object for depth and stencil attachment 
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    // check funzionamento
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Inizializza il VAO per riempire la minimappa
    float minimapWoodVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    unsigned int minimapWoodVAO;
    unsigned int minimapWoodVBO;
    glGenVertexArrays(1, &minimapWoodVAO);
    glGenBuffers(1, &minimapWoodVBO);
    glBindVertexArray(minimapWoodVAO);
    glBindBuffer(GL_ARRAY_BUFFER, minimapWoodVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(minimapWoodVertices), &minimapWoodVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    _VAO = minimapVAO;
    _minimapWoodVAO = minimapWoodVAO;
    _textureColorBuffer = textureColorBuffer;
    _framebuffer = framebuffer;
}

void Minimap::_initMinimapMarkers() {
    int steps = NUM_VERTICES_CIRCLE / 6;

    float circleVertices[NUM_VERTICES_CIRCLE] = {};
    float PI = 3.1416;
    float angle = 2 * PI / steps;

    float xPos = 0.0f;
    float yPos = 0.0f;
    float radius = 1.0f;

    float prevX = xPos;
    float prevY = yPos - radius;

    for (int i = 1; i <= steps; i++) {
        float newX = radius * (sin(angle * i));
        float newY = -radius * (cos(angle * i));

        int triangleIndex = (i - 1) * 6;
        circleVertices[triangleIndex] = xPos;
        circleVertices[triangleIndex + 1] = yPos;
        circleVertices[triangleIndex + 2] = newX;
        circleVertices[triangleIndex + 3] = newY;
        circleVertices[triangleIndex + 4] = prevX;
        circleVertices[triangleIndex + 5] = prevY;

        prevX = newX;
        prevY = newY;
    }

    unsigned int circleVAO;
    unsigned int circleVBO;
    glGenVertexArrays(1, &circleVAO);
    glGenBuffers(1, &circleVBO);
    glBindVertexArray(circleVAO);
    glBindBuffer(GL_ARRAY_BUFFER, circleVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(circleVertices), &circleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    _circleVAO = circleVAO;

    float personMarkerVertices[] = {
        // positions 
        -0.5f, -0.7f, 
         0.0f,  0.6f,  
         0.5f, -0.7f
    };

    unsigned int personVAO;
    unsigned int personVBO;
    glGenVertexArrays(1, &personVAO);
    glGenBuffers(1, &personVBO);
    glBindVertexArray(personVAO);
    glBindBuffer(GL_ARRAY_BUFFER, personVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(personMarkerVertices), &personMarkerVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

    _personVAO = personVAO;
}

void Minimap::_buildMinimap(const Camera& camera) {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.137f, 0.09f, 0.035f, 0.8f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _minimapWoodShader->use();
    glBindTexture(GL_TEXTURE_2D, _texture);
    glBindVertexArray(_minimapWoodVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    _minimapCircleShader->use();
    for (auto& const transform : _circleTransforms) {
        _minimapCircleShader->setMat4("transform", transform);
        _minimapCircleShader->setVec3("circleColor", glm::vec3(1.0f, 0.8f, 0.0f));
        glBindVertexArray(_circleVAO);
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_CIRCLE / 2);
    }

    float rotationAngle = atan2(camera.Front.x, camera.Front.z);
    rotationAngle = rotationAngle * 180 / M_PI;

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(-camera.Position.x / MAX_W_QUAD_MAP, camera.Position.z / MAX_W_QUAD_MAP, 0.0f));
    transform = glm::scale(transform, glm::vec3(0.09f, 0.09f, 0.09f));
    transform = glm::rotate(transform, (float)glm::radians(rotationAngle), glm::vec3(0.0f, 0.0f, 1.0f));
    _minimapCircleShader->setMat4("transform", transform);
    _minimapCircleShader->setVec3("circleColor", glm::vec3(1.0f, 0.0f, 0.0f));

    glBindVertexArray(_personVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Minimap::render(const Camera& camera, const LightUtils& lightUtils) {
    _buildMinimap(camera);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    _shader->use();
    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}