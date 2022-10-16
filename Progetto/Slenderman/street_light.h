#pragma once

#include "constants.h"
#include "light_utils.h"
#include "model_cache.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

class StreetLight : public ModelRenderable {
public:
    StreetLight(glm::mat4 transform);

    inline aabb* toAABB() const;

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

StreetLight::StreetLight(glm::mat4 transform) {
    _shader = ShaderCache::getInstance().findShader(EShader::streetLight);
    _model = ModelCache::getInstance().findModel(EModel::streetLight);
    _texture = TextureCache::getInstance().findTexture(ETexture::streetLight);

    _transform = transform;
}

aabb* StreetLight::toAABB() const {
    return aabb::fromModel(*(_model), _transform);
}

void StreetLight::render(const Camera& camera, const LightUtils& lightUtils) {
    lightUtils.initLightShader(_shader, camera);

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setMat4("model", _transform);

    _model->Draw(*_shader);
}
