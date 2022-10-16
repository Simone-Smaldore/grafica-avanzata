#pragma once

#include <map>
#include <string>

#include "raudio/raudio.h"

enum class ESfx {
    lightOn,
    footstep1,
    footstep2,
    footstep3,
    paper,
};

enum class EMusic {
    background,
    whiteNoise,
    highFear,
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

    void loadMusic(EMusic key, std::string filePath, bool autoplay = false, float volume = 1.0f);

    void playMusicFromBeginning(EMusic key, float volume = 1.0f) const;

    void pauseMusic(EMusic key) const;

    void setMusicVolume(EMusic key, float volume) const;

    void loadSfx(ESfx key, std::string filePath, float volume = 1.0f);

    void playSfx(ESfx key, bool setVolume = false, float volume = 1.0f) const;

    void playRandomFootstep() const;

    bool isPlayingFootstep() const;

    void process() const;

    void destroy();

    inline bool has(EMusic key) const { return _musicCache.find(key) != _musicCache.end(); }

};

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

inline void AudioManager::initAudio() const {
    InitAudioDevice();
}

void AudioManager::loadMusic(EMusic key, std::string filePath, bool autoplay, float volume) {
    Music music = LoadMusicStream(filePath.c_str());
    if (autoplay) {
        SetMusicVolume(music, volume);
        PlayMusicStream(music);
    }
    _musicCache[key] = music;
}

void AudioManager::playMusicFromBeginning(EMusic key, float volume) const {
    auto music = _musicCache.find(key);
    if (music == _musicCache.end())
        return;
    if (volume > 1.0)
        volume = 1.0;
    SetMusicVolume(music->second, volume);
    SeekMusicStream(music->second, 0.0f);
    PlayMusicStream(music->second);
}

void AudioManager::pauseMusic(EMusic key) const {
    auto music = _musicCache.find(key);
    if (music == _musicCache.end())
        return;
    PauseMusicStream(music->second);
}

void AudioManager::setMusicVolume(EMusic key, float volume) const {
    if (volume < 0)
        volume = 0;
    auto music = _musicCache.find(key);
    if (music == _musicCache.end())
        return;
    SetMusicVolume(music->second, volume);
}

void AudioManager::loadSfx(ESfx key, std::string filePath, float volume) {
    Sound sound = LoadSound(filePath.c_str());
    SetSoundVolume(sound, volume);
    _sfxCache[key] = sound;
}

void AudioManager::playSfx(ESfx key, bool setVolume, float volume) const {
    auto sfx = _sfxCache.find(key);
    if (sfx == _sfxCache.end())
        return;

    if (setVolume) {
        if (volume > 1.0)
            volume = 1.0;
        SetSoundVolume(sfx->second, volume);
    }

    PlaySound(sfx->second);
}

void AudioManager::playRandomFootstep() const {
    int footstepIndex = static_cast<int>(ESfx::footstep1) + (std::rand() % (static_cast<int>(ESfx::footstep3) - static_cast<int>(ESfx::footstep1) + 1));
    PlaySound(_sfxCache.find(static_cast<ESfx>(footstepIndex))->second);
}

bool AudioManager::isPlayingFootstep() const {
    for (int i = static_cast<int>(ESfx::footstep1); i <= static_cast<int>(ESfx::footstep3); i++) {
        auto sfx = _sfxCache.find(static_cast<ESfx>(i));
        if (sfx == _sfxCache.end())
            continue;
        if (IsSoundPlaying(sfx->second))
            return true;
    }
    return false;
}

void AudioManager::process() const {
    for (const auto& music : _musicCache) {
        if (!IsMusicStreamPlaying(music.second))
            continue;

        if (GetMusicTimePlayed(music.second) > GetMusicTimeLength(music.second))
            SeekMusicStream(music.second, 0.0f);

        UpdateMusicStream(music.second);
    }
}

void AudioManager::destroy() {
    for (const auto& music : _musicCache)
        UnloadMusicStream(music.second);

    for (const auto& sfx : _sfxCache)
        UnloadSound(sfx.second);

    CloseAudioDevice();
}