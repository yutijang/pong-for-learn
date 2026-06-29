#include "scenes/GameplayScene.hpp"

#include "resources/MusicLoopMode.hpp"
#include "scenes/SceneRequest.hpp"
#include "ui/PauseOverlay.hpp"
#include "world/GameLevel.hpp"
#include "world/LevelData.hpp"
#include "world/LevelLoader.hpp"

#include <filesystem>
#include <memory>
#include <optional>
#include <raylib.h>

void GameplayScene::processInput() {
    if (IsKeyPressed(KEY_ESCAPE)) {
        m_paused = !m_paused;

        if (m_paused) {
            m_audioManager.pauseMusic();
        } else {
            m_audioManager.resumeMusic();
        }

        return;
    }

    if (m_paused) {
        PauseOverlay::MenuResult result = m_pauseOverlay->processInput();
        if (result == PauseOverlay::MenuResult::Resume) {
            m_paused = false;
            m_audioManager.resumeMusic();
        } else if (result == PauseOverlay::MenuResult::MainMenu) {
            m_pendingSceneRequest = SceneRequest{SceneId::Menu};
        }
    } else {
        if (m_currentLevel) {
            m_currentLevel->processInput();
        }
    }
}

void GameplayScene::update(float deltaTime) {
    if (m_paused) {
        m_pauseOverlay->update(deltaTime);
        return;
    }

    if (m_currentLevel) {
        m_currentLevel->update(deltaTime);
    }

    if (m_state == GameplayState::VictoryMusic) {
        if (!m_audioManager.isCurrentMusicStreamPlaying()) {
            m_audioManager.playMusic("level1_bgm");

            m_state = GameplayState::BGMMusic;
        }
    }
}

void GameplayScene::draw() const {
    if (m_currentLevel) {
        m_currentLevel->draw();
    }

    if (m_paused) {
        m_pauseOverlay->draw();
    }
}

void GameplayScene::onEnter() {
    if (!loadAssets()) {
        return;
    }

    auto const* const scoreFont = m_assetManager.font("level1_font_lg");
    auto const* const countdownFont = m_assetManager.font("level1_font_countdown");

    LevelResources resources{.scoreFont = *scoreFont, .countdownFont = *countdownFont};

    auto result = LevelLoader::load("assets/levels/level1.cfg");
    if (result) {
        m_currentLevel = std::make_unique<GameLevel>(result.value(), resources);

        connectLevelEvents();

        m_currentLevel->onEnter();
    } else {
        char const* errMsg{};
        switch (result.error()) {
            case LoadError::OpenFileFailed: {
                errMsg = "OpenFileFailed";
                break;
            }
            case LoadError::FileStructWrong: {
                errMsg = "FileStructWrong";
                break;
            }
        }
        TraceLog(LOG_FATAL, errMsg);
        return;
    }

    m_pauseOverlay.emplace(m_megaFont, m_midleFont);

    m_audioManager.playMusic("level1_bgm");
}

void GameplayScene::onExit() {
    if (m_currentLevel) {
        m_currentLevel->onExit();
    }

    m_audioManager.stopMusic();

    m_currentLevel.reset();

    m_assetManager.unloadAssetGroup("level1");
}

std::optional<SceneRequest> GameplayScene::consumeSceneRequest() {
    auto request = m_pendingSceneRequest;
    m_pendingSceneRequest.reset();
    return request;
}

// Helper
bool GameplayScene::loadAssets() {
    if (!m_assetManager.loadAssetGroup("level1")) {
        TraceLog(LOG_FATAL, "loadAssetGroup level1");
        return false;
    }

    m_megaFont = m_assetManager.font("level1_font_mg");
    if (m_megaFont == nullptr) {
        TraceLog(LOG_FATAL, "load font level1_font_lg failed");
        return false;
    }

    m_midleFont = m_assetManager.font("level1_font_md");
    if (m_midleFont == nullptr) {
        TraceLog(LOG_FATAL, "load font level1_font_md failed");
        return false;
    }

    m_smallFont = m_assetManager.font("level1_font_sm");
    if (m_smallFont == nullptr) {
        TraceLog(LOG_FATAL, "load font level1_font_sm failed");
        return false;
    }

    return true;
}

void GameplayScene::connectLevelEvents() {
    m_currentLevel->onLevelEvent([this](LevelEvent event) {
        switch (event) {
            case LevelEvent::WallHit   : m_audioManager.playSound("level1_wall_hit"); break;
            case LevelEvent::PaddleHit : m_audioManager.playSound("level1_paddle_hit"); break;
            case LevelEvent::WhenScored: m_audioManager.playSound("level1_when_scored"); break;
            case LevelEvent::GameSetWin: {
                m_audioManager.playMusic("level1_game_set_win", MusicLoopMode::NoLoop);
                m_state = GameplayState::VictoryMusic;
                break;
            }
            default: break;
        }
    });
}
