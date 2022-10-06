#pragma once

#include "constants.h"
#include "light_utils.h"
#include "model_cache.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

class StreetLight : public ModelRenderable {
private:
    LightUtils& _lightUtils;

public:
    StreetLight(LightUtils& lightUtils);

    virtual void render(const Camera& camera) override;
};

StreetLight::StreetLight(LightUtils& lightUtils) : _lightUtils(lightUtils) {
    _shader = ShaderCache::getInstance().findShader(EShader::streetLight);
    _model = ModelCache::getInstance().findModel(EModel::streetLight);
    _texture = TextureCache::getInstance().findTexture(ETexture::streetLight);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(STREETLIGHT_POI_OFFSET, 0.0f, STREETLIGHT_POI_OFFSET));
    transform = glm::translate(transform, glm::vec3(40.0f, -4.0f, 40.0f));
    transform = glm::scale(transform, glm::vec3(0.015f, 0.015f, 0.015f));
    _transform = transform
}

void StreetLight::render(const Camera& camera) {
    //nTODO: _lightUtils.initLightShader(_shader, lightOn, camera);
    _lightUtils.initLightShader(_shader, false, camera);

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setMat4("model", _transform);

    _model->Draw(*_shader);
}
