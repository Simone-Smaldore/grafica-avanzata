#pragma once

#include <iostream>
#include <map>

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum class ETexture {
    slenderMan,
    floor,
    streetLight,
    poi1,
    poi2,
    poi3,
    poi4,
    poi5,
    poi6,
    poi7,
    poi8,
    minimap,
    fence
};

class TextureCache {
private:
    std::map<ETexture, unsigned int> _textureCache;

    unsigned int _loadTexture(char const* path);

    TextureCache() {}

public:
    TextureCache(TextureCache const&) = delete;
    void operator=(TextureCache const&) = delete;

    static TextureCache& getInstance();

    void registerTexture(ETexture key, const char* path);

    unsigned int findTexture(ETexture key);

    void clear();
};

TextureCache& TextureCache::getInstance() {
    static TextureCache instance;
    return instance;
}

void TextureCache::registerTexture(ETexture key, const char* path) {
    unsigned int value = _loadTexture(path);
    _textureCache[key] = value;
}

unsigned int TextureCache::findTexture(ETexture key) {
    return _textureCache[key];
}

void TextureCache::clear() {
    _textureCache.clear();
}

unsigned int TextureCache::_loadTexture(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        //std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
