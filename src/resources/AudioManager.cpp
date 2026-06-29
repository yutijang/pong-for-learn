#include "AudioManager.hpp"

#include "resources/MusicLoopMode.hpp"

#include <raylib.h>
#include <string_view>

void AudioManager::update() {
    if (m_currentMusic == nullptr || m_musicState != MusicState::Playing) {
        return;
    }

    UpdateMusicStream(*m_currentMusic);
}

void AudioManager::playSound(std::string_view assetId) {
    auto const* sound = m_assetManager.sound(assetId);
    if (sound == nullptr) {
        return;
    }

    // finalMusicVolume = masterVolume * musicVolume;
    // finalSoundVolume = masterVolume * soundVolume;
    float const finalVolume = m_muted ? 0.0F : m_masterVolume * m_soundVolume;

    SetSoundVolume(*sound, finalVolume);

    PlaySound(*sound);
}

void AudioManager::playMusic(std::string_view assetId, MusicLoopMode loopMode) {
    auto* music = m_assetManager.music(assetId);
    if (music == nullptr) {
        return;
    }

    music->looping = (loopMode == MusicLoopMode::Loop);

    // Nếu đang có nhạc cũ thì dừng nó
    if (m_currentMusic != nullptr) {
        StopMusicStream(*m_currentMusic);
    }

    // Cập nhật trạng thái mới
    m_currentMusic = music;

    applyMusicVolume();

    PlayMusicStream(*m_currentMusic);

    m_musicState = MusicState::Playing;
}

void AudioManager::stopMusic() {
    if (m_musicState == MusicState::Stopped) {
        return;
    }

    StopMusicStream(*m_currentMusic);

    m_currentMusic = nullptr;

    m_musicState = MusicState::Stopped;
}

void AudioManager::pauseMusic() {
    if (m_musicState != MusicState::Playing) {
        return;
    }

    PauseMusicStream(*m_currentMusic);

    m_musicState = MusicState::Paused;
}

void AudioManager::resumeMusic() {
    if (m_musicState != MusicState::Paused) {
        return;
    }

    ResumeMusicStream(*m_currentMusic);

    m_musicState = MusicState::Playing;
}

void AudioManager::applyMusicVolume() {
    if (m_currentMusic == nullptr) {
        return;
    }

    float const finalVolume = m_muted ? 0.0F : m_masterVolume * m_musicVolume;

    SetMusicVolume(*m_currentMusic, finalVolume);
}
