#pragma once

#include "renderable.h"
#include "texture_cache.h"
#include "shader_cache.h"

class Minimap : public VAORenderable {
private:
    unsigned int _framebuffer;
    unsigned int _textureColorBuffer;
    unsigned int _minimapWoodVAO;
    Shader* _minimapWoodShader;

    void _initMinimap();
    void _buildMinimap();

public:
    Minimap();

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

Minimap::Minimap() {
    _texture = TextureCache::getInstance().findTexture(ETexture::minimap);

    _shader = ShaderCache::getInstance().findShader(EShader::minimap);
    _minimapWoodShader = ShaderCache::getInstance().findShader(EShader::minimapWood);

    _initMinimap();
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

void Minimap::_buildMinimap() {
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    glDisable(GL_DEPTH_TEST);
    glClearColor(0.137f, 0.09f, 0.035f, 0.8f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _minimapWoodShader->use();
    glBindTexture(GL_TEXTURE_2D, _texture);
    glBindVertexArray(_minimapWoodVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // nTODO: gestire i puntini sulla minimappa
    /*circleMinimapShader.use();
    glm::mat4 transform = glm::mat4(1.0f);
    for (int i = 0; i < pointOfinterestTranslationVec.size(); i++) {
        glm::vec3 poitv = pointOfinterestTranslationVec[i];
        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(-poitv.x / MAX_W_QUAD_MAP, poitv.z / MAX_W_QUAD_MAP, 0.0f));
        transform = glm::scale(transform, glm::vec3(0.03f, 0.03f, 0.03f));
        circleMinimapShader.setMat4("transform", transform);
        circleMinimapShader.setVec3("circleColor", glm::vec3(1.0f, 0.8f, 0.0f));
        glBindVertexArray(circleVAO);
        glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_CIRCLE / 2);
    }

    transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(-camera.Position.x / MAX_W_QUAD_MAP, camera.Position.z / MAX_W_QUAD_MAP, 0.0f));
    transform = glm::scale(transform, glm::vec3(0.04f, 0.04f, 0.04f));
    circleMinimapShader.setMat4("transform", transform);
    circleMinimapShader.setVec3("circleColor", glm::vec3(1.0f, 0.0f, 0.0f));
    glBindVertexArray(circleVAO);
    glDrawArrays(GL_TRIANGLES, 0, NUM_VERTICES_CIRCLE / 2);*/

    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Minimap::render(const Camera& camera, const LightUtils& lightUtils) {
    _buildMinimap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    _shader->use();
    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}