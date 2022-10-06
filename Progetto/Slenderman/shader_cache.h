#pragma once

#include <map>

#include "shader_m.h"

enum class EShader {
    slenderMan,
    floor,
    streetLight,
    tree,
    grass
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

    void clear();
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

void ShaderCache::clear() {
    for (auto pair : _shaderCache)
        delete pair.second;
     
     _shaderCache.clear();
}