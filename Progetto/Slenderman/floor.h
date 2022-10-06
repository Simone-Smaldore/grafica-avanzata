#pragma once

#include "light_utils.h"
#include "renderable.h"
#include "shader_cache.h"
#include "texture_cache.h"

class Floor : public VAORenderable {
private:
    LightUtils& _lightUtils;

public:
    Floor(LightUtils& lightUtils);

    virtual void render(const Camera& camera) override;
};

Floor::Floor(LightUtils& lightUtils) : _lightUtils(lightUtils) {
    _shader = ShaderCache::getInstance().findShader(EShader::floor);
    _VAO = VAORenderable::_initRectVAO(2000.0f);
    _texture = TextureCache::getInstance().findTexture(ETexture::floor);

    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(0.0f, -4.0f, 0.0f));
    _transform = transform;
}

void Floor::render(const Camera& camera) {
    //nTODO: _lightUtils.initLightShader(_shader, lightOn, camera);
    _lightUtils.initLightShader(_shader, false, camera);
    _shader->use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _texture);
    
    _shader->setMat4("model", _transform);
    // nTODO: view e projection possono essere calcolati una sola volta all'inizio del frame
    _shader->setMat4("view", camera.GetViewMatrix());
    _shader->setMat4("projection", camera.GetProjection());
    
    glBindVertexArray(_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
