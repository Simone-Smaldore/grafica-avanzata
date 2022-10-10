#pragma once

#include "aabb.h"
#include "light_utils.h"
#include "model_cache.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

class RenderablePOI : public ModelRenderable {
public:
    RenderablePOI(ETexture texture, EModel model, glm::mat4 transform);

    inline aabb* toAABB() const;

    virtual void render(const Camera& camera, const LightUtils& lightUtils) override;
};

RenderablePOI::RenderablePOI(ETexture texture, EModel model, glm::mat4 transform) {
    _shader = ShaderCache::getInstance().findShader(EShader::poi);
    _model = ModelCache::getInstance().findModel(model);
    _texture = TextureCache::getInstance().findTexture(texture);

    _transform = transform;
}

aabb* RenderablePOI::toAABB() const {
    return aabb::fromModel(*(_model), _transform);
}

void RenderablePOI::render(const Camera& camera, const LightUtils& lightUtils) {
    lightUtils.initLightShader(_shader, camera);

    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);

    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());
    _shader->setMat4("model", _transform);

    _model->Draw(*_shader);
}