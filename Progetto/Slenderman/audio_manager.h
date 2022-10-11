#pragma once

#include <map>
#include <string>

#include "raudio/raudio.h"

enum class ESfx {

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
    
    CloseAudioDevice();
}