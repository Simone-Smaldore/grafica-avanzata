#pragma once

#include "GLFW/glfw3.h"

#include "constants.h"
#include "renderable.h"
#include "shader_cache.h"

class FearRenderable : public VAORenderable {
private:
    const glm::vec2 _kResolution = glm::vec2(SCR_WIDTH, SCR_HEIGHT);
    float& _fearFactor;

public:
    FearRenderable(float& fearFactor);

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

FearRenderable::FearRenderable(float& fearFactor) : _fearFactor(fearFactor) {
    _shader = ShaderCache::getInstance().findShader(EShader::fear);

    float vertices[] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };

    unsigned int VBO, VAO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    _VAO = VAO;
}

void FearRenderable::render(const Camera& camera, const LightUtils& lightUtils) {
    if (_fearFactor - 0.05 < 0.0)
        return;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    _shader->use();
    _shader->setFloat("time", glfwGetTime());
    _shader->setVec2("resolution", _kResolution);
    _shader->setFloat("fearFactor", _fearFactor);
    
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);

    glDisable(GL_BLEND);
}