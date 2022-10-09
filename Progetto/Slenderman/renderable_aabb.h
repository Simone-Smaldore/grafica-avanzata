#pragma once

#include "aabb.h"
#include "constants.h"
#include "renderable.h"
#include "shader_cache.h"

class RenderableAABB : public VAORenderable {
private:
    const aabb& _staticAABB;

public:
    RenderableAABB(const aabb& staticAABB);

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

RenderableAABB::RenderableAABB(const aabb& staticAABB) : _staticAABB(staticAABB) {
    _shader = ShaderCache::getInstance().findShader(EShader::aabb);

    float vertices[] = {
        _staticAABB.getMin().x, _staticAABB.getMin().y, _staticAABB.getMin().z,
        _staticAABB.getMax().x, _staticAABB.getMin().y, _staticAABB.getMin().z,
        _staticAABB.getMax().x, _staticAABB.getMax().y, _staticAABB.getMin().z,
        _staticAABB.getMin().x, _staticAABB.getMax().y, _staticAABB.getMin().z,
        _staticAABB.getMin().x, _staticAABB.getMin().y, _staticAABB.getMax().z,
        _staticAABB.getMax().x, _staticAABB.getMin().y, _staticAABB.getMax().z,
        _staticAABB.getMax().x, _staticAABB.getMax().y, _staticAABB.getMax().z,
        _staticAABB.getMin().x, _staticAABB.getMax().y, _staticAABB.getMax().z,
    };

    unsigned int indices[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        0, 4, 1, 5, 2, 6, 3, 7
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

void RenderableAABB::render(const Camera& camera, const LightUtils& lightUtils) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);

    _shader->use();
    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setVec4("color", _staticAABB.hasIntersection() ? RED : AABB_COLOR);

    glBindVertexArray(_VAO);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}
