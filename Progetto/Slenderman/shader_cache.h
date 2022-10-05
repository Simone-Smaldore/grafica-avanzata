#pragma once

#include <map>

#include "shader_m.h"

enum class EShader {
    slenderMan
};

class ShaderCache {
private:
    std::map<EShader, Shader*> _shaderCache;

    ShaderCache() {}

public:
    ShaderCache(ShaderCache const&) = delete;
    void operator=(ShaderCache const&) = delete;

    static ShaderCache& getInstance();

    void registerShader(EShader key, Shader* value);

    Shader* findShader(EShader key);
};

ShaderCache& ShaderCache::getInstance() {
    static ShaderCache instance;
    return instance;
}

void ShaderCache::registerShader(EShader key, Shader* value) {
    _shaderCache[key] = value;
}

Shader* ShaderCache::findShader(EShader key) {
    return _shaderCache[key];
}