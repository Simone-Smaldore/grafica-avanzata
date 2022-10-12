#pragma once

#include <map>
#include <string>

#include "raudio/raudio.h"

enum class ESfx {
    lightOn
};

enum class EMusic {
    background
};

class AudioManager {
private:
    std::map<ESfx, Sound> _sfxCache;
    std::map<EMusic, Music> _musicCache;

    bool audioInitialized = false;

    AudioManager() {}

public:
    AudioManager(AudioManager const&) = delete;
    void operator=(AudioManager const&) = delete;

    static AudioManager& getInstance();

    inline void initAudio() const;

    void loadMusic(EMusic key, std::string filePath, bool autoplay = false);

    void loadSfx(ESfx key, std::string filePath);

    void playSfx(ESfx key, float volume = 1.0f) const;

    void process() const;

    void destroy();
};

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

inline void AudioManager::initAudio() const {
    InitAudioDevice();
}

void AudioManager::loadMusic(EMusic key, std::string filePath, bool autoplay) {
    Music music = LoadMusicStream(filePath.c_str());
    if (autoplay)
        PlayMusicStream(music);
    _musicCache[key] = music;
}

void AudioManager::loadSfx(ESfx key, std::string filePath) {
    Sound sound = LoadSound(filePath.c_str());
    _sfxCache[key] = sound;
}

void AudioManager::playSfx(ESfx key, float volume) const {
    auto sfx = _sfxCache.find(key);
    if (sfx == _sfxCache.end())
        return;
    if (volume > 1.0)
        volume = 1.0;
    SetSoundVolume(sfx->second, volume);
    PlaySound(sfx->second);
}

void AudioManager::process() const {
    for (const auto& music : _musicCache)
        UpdateMusicStream(music.second);
    // TODO: Always loop music?
/*if (GetMusicTimePlayed(backgroundMusic) > GetMusicTimeLength(backgroundMusic))
    SeekMusicStream(backgroundMusic, 0);*/
}

void AudioManager::destroy() {
    for (const auto& music : _musicCache)
        UnloadMusicStream(music.second);

    for (const auto& sfx : _sfxCache)
        UnloadSound(sfx.second);

    CloseAudioDevice();
}