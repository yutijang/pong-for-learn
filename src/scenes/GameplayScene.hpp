#pragma once

#include "IScene.hpp"
#include "SceneRequest.hpp"
#include "resources/AssetManager.hpp"
#include "resources/AudioManager.hpp"
#include "ui/PauseOverlay.hpp"
#include "world/ILevel.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <raylib.h>

class GameplayScene final : public IScene {
  public:
    explicit GameplayScene(AssetManager& assetManager, AudioManager& audioManager)
        : m_assetManager(assetManager), m_audioManager(audioManager) {}

    void processInput() override;
    void update(float deltaTime) override;
    void draw() const override;

    void onEnter() override;
    void onExit() override;

    [[nodiscard]] std::optional<SceneRequest> consumeSceneRequest() override;

  private:
    // Helper
    bool loadAssets();

    void connectLevelEvents();

    AssetManager& m_assetManager;
    AudioManager& m_audioManager;

    std::unique_ptr<ILevel> m_currentLevel;
    std::optional<SceneRequest> m_pendingSceneRequest;

    bool m_paused{};
    enum class PauseMenuItem : std::uint8_t { Resume, MainMenu };
    PauseMenuItem m_selectedPauseItem{PauseMenuItem::Resume};
    Font const* m_smallFont{};
    Font const* m_midleFont{};
    Font const* m_megaFont{};

    std::optional<PauseOverlay> m_pauseOverlay;

    enum class GameplayState : std::uint8_t { VictoryMusic, BGMMusic };
    GameplayState m_state{GameplayState::BGMMusic};
};
