#pragma once

#include "glm/glm.hpp"

#include "constants.h"
#include "renderable.h"
#include "texture_cache.h"
#include "shader_cache.h"

class FullsceenImage : public VAORenderable {
private:
    unsigned int _framebuffer;
    unsigned int _textureColorBuffer;
    unsigned int _VAO;
    Shader* _shader;
    void _initImage();

public:
    FullsceenImage(ETexture imageTexture);

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;

};

FullsceenImage::FullsceenImage(ETexture imageTexture) {
    _texture = TextureCache::getInstance().findTexture(imageTexture);
    _shader = ShaderCache::getInstance().findShader(EShader::fullScreenImage);
    _initImage();
}

void FullsceenImage::_initImage() {
    float vertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 0.0f,
        -1.0f, -1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 1.0f,

        -1.0f,  1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 1.0f,
         1.0f,  1.0f,  1.0f, 0.0f
    };

    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    _VAO = VAO;
}


void FullsceenImage::render(const Camera& camera, const LightUtils& lightUtils) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_DEPTH_TEST);
    _shader->use();
    glBindVertexArray(_VAO);
    glBindTexture(GL_TEXTURE_2D, _texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_DEPTH_TEST);
}