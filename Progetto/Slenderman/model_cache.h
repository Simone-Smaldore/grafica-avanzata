#pragma once

#include <map>

#include "model.h"

enum class EModel {
    slenderMan,
    streetLight,
    tree,
    grass,
    poi1,
    poi2,
    poi3,
    poi4,
    poi5,
    poi6,
    poi7,
    poi8,
    fence
};

class ModelCache {
private:
    std::map<EModel, Model*> _modelCache;

    ModelCache() {}

public:
    ModelCache(ModelCache const&) = delete;
    void operator=(ModelCache const&) = delete;

    static ModelCache& getInstance();

    void registerModel(EModel key, Model* value);

    Model* findModel(EModel key);

    void clear();

    inline bool has(EModel key) const { return _modelCache.find(key) != _modelCache.end(); } 
};

ModelCache& ModelCache::getInstance() {
    static ModelCache instance;
    return instance;
}

void ModelCache::registerModel(EModel key, Model* value) {
    if (_modelCache.find(key) != _modelCache.end())
        return;

    _modelCache[key] = value;
}

Model* ModelCache::findModel(EModel key) {
    return _modelCache[key];
}

void ModelCache::clear() {
    for (auto pair : _modelCache)
        delete pair.second;

    _modelCache.clear();
}