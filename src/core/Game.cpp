#include "Game.hpp"

#include "GameConfig.hpp"
#include "scenes/GameplayScene.hpp"
#include "scenes/IScene.hpp"
#include "scenes/MenuScene.hpp"
#include "scenes/SceneRequest.hpp"

#include <memory>
#include <raylib.h>

bool Game::initializer() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(GameConfig::WINDOW_WIDTH, GameConfig::WINDOW_HEIGHT, GameConfig::TITLE);
    SetTargetFPS(GameConfig::FPS);
    InitAudioDevice();

    SetExitKey(KEY_NULL);

    if (!m_assetManager.buildRegistry("assets/groups")) {
        TraceLog(LOG_FATAL, "buildRegistry failed");
    }

    m_currentScene = std::make_unique<MenuScene>(m_assetManager, m_audioManager);
    m_currentScene->onEnter();

    return true;
}

void Game::run() {
    while (m_running && !WindowShouldClose()) {
        float const deltaTime{GetFrameTime()};

        m_audioManager.update();
        m_currentScene->processInput();
        m_currentScene->update(deltaTime);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        m_currentScene->draw();

        EndDrawing();

        processSceneRequest();
    }
}

/* Mục tiêu:
 * 1. Giải phóng Scene hiện tại
 * 2. Giải phóng AssetManager
 * 3. Tắt Audio
 * 4. Tắt Window
 */
void Game::shutdown() {
    m_currentScene.reset();
    m_assetManager.unloadAll();

    CloseAudioDevice();
    CloseWindow();
}

std::unique_ptr<IScene> Game::createScene(SceneId sceneId) {
    switch (sceneId) {
        case SceneId::Menu: {
            return std::make_unique<MenuScene>(m_assetManager, m_audioManager);
        }
        case SceneId::Gameplay: {
            return std::make_unique<GameplayScene>(m_assetManager, m_audioManager);
        }
        case SceneId::None:
        case SceneId::Settings:
        case SceneId::GameOver:
        case SceneId::Credits:
        case SceneId::Exit    : {
            return nullptr;
        }
    }

    return nullptr;
}

void Game::processSceneRequest() {
    auto const request = m_currentScene->consumeSceneRequest();

    if (!request.has_value()) {
        return;
    }

    switch (request->target) {
        case SceneId::Menu:
        case SceneId::Gameplay: {
            m_currentScene->onExit();

            m_currentScene = createScene(request->target);
            if (m_currentScene) {
                m_currentScene->onEnter();
            }

            break;
        }

        case SceneId::Exit: {
            m_running = false;
            break;
        }

        case SceneId::None:
        case SceneId::Settings:
        case SceneId::GameOver:
        case SceneId::Credits : break;
    }
}
