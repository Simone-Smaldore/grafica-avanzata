#pragma once

#include "light_utils.h"
#include "model_cache.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

class SlenderMan : public ModelRenderable {
public:
    SlenderMan();

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;

    void setTransform(glm::mat4 transform) {
        _transform = transform;
    }
};

SlenderMan::SlenderMan(){
    _shader = ShaderCache::getInstance().findShader(EShader::slenderMan);
    _model = ModelCache::getInstance().findModel(EModel::slenderMan);
    _texture = TextureCache::getInstance().findTexture(ETexture::slenderMan);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, -0.8f, -10.0f));
    transform = glm::scale(transform, glm::vec3(0.01f, 0.01f, 0.01f));
    _transform = transform;
}

void SlenderMan::render(const Camera& camera, const LightUtils& lightUtils) {
    lightUtils.initLightShader(_shader, camera);

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setMat4("model", _transform);

    _model->Draw(*_shader);
}
