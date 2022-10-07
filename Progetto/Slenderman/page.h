#pragma once

#include "constants.h"
#include "light_utils.h"
#include "renderable.h"
#include "shader_m.h"
#include "shader_cache.h"
#include "texture_cache.h"

class Page : public VAORenderable {
private:
    bool _collected = false;
    bool _framed = true;

    Shader* _shaderSingleColor;

    glm::mat4 _singleColorTransform;
    glm::vec3 _relatedPOITranslation;

public:
    Page(ETexture texture, glm::vec3 poiTranslation);

    inline void setFramed(bool framed);

    inline void setCollected(bool collected);

    inline const glm::vec3& getRelatedPOITranslation() const;

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

Page::Page(ETexture texture, glm::vec3 poiTranslation) {
    _texture = TextureCache::getInstance().findTexture(texture);
    _shader = ShaderCache::getInstance().findShader(EShader::page);
    _shaderSingleColor = ShaderCache::getInstance().findShader(EShader::singleColor);
    _relatedPOITranslation = poiTranslation;
    _VAO = VAORenderable::_initRectVAO(1.0f);

    glm::mat4 transform = glm::mat4(1.0f);
    //TODO: Decidere se la pagina deve essere attaccata al lampione o al punto di interesse
    transform = glm::translate(transform, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
    transform = glm::translate(transform, poiTranslation);
    transform = glm::translate(transform, glm::vec3(0.7f, 4.3f, 0.0f));
    transform = glm::scale(transform, glm::vec3(0.6f, 0.6f, 0.4f));
    transform = glm::rotate(transform, (float)glm::radians(5.0), glm::vec3(0.0f, 1.0f, 1.0f));
    transform = glm::rotate(transform, (float)glm::radians(270.0), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, (float)glm::radians(90.0), glm::vec3(0.0f, 0.0f, 1.0f));
    _transform = transform;

    float scale = 1.15f;
    _singleColorTransform = glm::scale(_transform, glm::vec3(scale, scale, scale));
}

void Page::setFramed(bool framed) {
    _framed = framed;
}

void Page::setCollected(bool collected) {
    _collected = collected;
}

inline const glm::vec3& Page::getRelatedPOITranslation() const {
    return _relatedPOITranslation;
}

void Page::render(const Camera& camera, const LightUtils& lightUtils) {
    if (_collected)
        return;

    lightUtils.initLightShader(_shader, camera);
    _shader->use();
    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    _shader->setMat4("model", _transform);

    if (_framed) {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 1, 0xFF);
        glStencilMask(0xFF);
    }

    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if (_framed) {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

        glStencilMask(0x00);
        glDisable(GL_DEPTH_TEST);
        _shaderSingleColor->use();
        _shaderSingleColor->setMat4("view", camera.GetViewMatrix());
        _shaderSingleColor->setMat4("projection", camera.GetProjection());
        _shaderSingleColor->setMat4("model", _singleColorTransform);
        glBindVertexArray(_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    }

    glEnable(GL_DEPTH_TEST);

    if (_framed)
        RenderText("Click left mouse button to collect the page", (SCR_WIDTH / 2) - 275.0f, 200.0f, 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));
}