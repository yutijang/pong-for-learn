#pragma once

#include "resources/AssetManager.hpp"
#include "resources/MusicLoopMode.hpp"

#include <cstdint>
#include <raylib.h>
#include <string_view>

class AudioManager {
  public:
    explicit AudioManager(AssetManager& assetManager) : m_assetManager(assetManager) {}

    void update();

    void playSound(std::string_view assetId);
    void playMusic(std::string_view assetId, MusicLoopMode loopMode = MusicLoopMode::Loop);

    void stopMusic();
    void pauseMusic();
    void resumeMusic();

    [[nodiscard]]
    bool isMusicPlaying() const noexcept {
        return m_musicState == MusicState::Playing;
    }

    [[nodiscard]]
    bool isMusicPaused() const noexcept {
        return m_musicState == MusicState::Paused;
    }

    [[nodiscard]]
    bool isMusicStopped() const noexcept {
        return m_musicState == MusicState::Stopped;
    }

    [[nodiscard]]
    bool isMusicAssetPlaying(std::string_view assetId) const {
        return IsMusicStreamPlaying(*m_assetManager.music(assetId));
    }

    [[nodiscard]]
    bool isCurrentMusicStreamPlaying() const {
        return (m_currentMusic != nullptr) && IsMusicStreamPlaying(*m_currentMusic);
    }

    void setMasterVolume(float volume) noexcept {
        m_masterVolume = volume;

        applyMusicVolume();
    }

    void setMusicVolume(float volume) noexcept {
        m_musicVolume = volume;

        applyMusicVolume();
    }

    void setSoundVolume(float volume) noexcept {
        m_soundVolume = volume;
    }

    [[nodiscard]]
    float masterVolume() const noexcept {
        return m_masterVolume;
    }

    [[nodiscard]]
    float musicVolume() const noexcept {
        return m_musicVolume;
    }

    [[nodiscard]]
    float soundVolume() const noexcept {
        return m_soundVolume;
    }

    void mute() noexcept {
        m_muted = true;

        applyMusicVolume();
    }

    void unmute() noexcept {
        m_muted = false;

        applyMusicVolume();
    }

    [[nodiscard]]
    bool isMuted() const noexcept {
        return m_muted;
    }

  private:
    void applyMusicVolume();

    enum class MusicState : std::uint8_t { Stopped, Playing, Paused };

    AssetManager& m_assetManager;

    Music* m_currentMusic{};

    float m_masterVolume{1.0F};
    float m_musicVolume{1.0F};
    float m_soundVolume{1.0F};

    bool m_muted{};
    MusicState m_musicState{MusicState::Stopped};
};
