#pragma once

#include <map>

#include "model.h"

enum class EModel {
    slenderMan
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
};

ModelCache& ModelCache::getInstance() {
    static ModelCache instance;
    return instance;
}

void ModelCache::registerModel(EModel key, Model* value) {
    _modelCache[key] = value;
}

Model* ModelCache::findModel(EModel key) {
    return _modelCache[key];
}