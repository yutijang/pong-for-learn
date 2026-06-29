#pragma once

#include "IScene.hpp"
#include "SceneRequest.hpp"
#include "resources/AssetManager.hpp"
#include "resources/AudioManager.hpp"
#include "ui/Label.hpp"
#include "ui/MenuList.hpp"

#include <cstdint>
#include <optional>
#include <raylib.h>

class MenuScene final : public IScene {
  public:
    explicit MenuScene(AssetManager& assetManager, AudioManager& audioManager)
        : m_assetManager(assetManager), m_audioManager(audioManager) {}

    void processInput() override;

    void update(float deltaTime) override;

    void draw() const override;

    void onEnter() override;
    void onExit() override;

    // Scene phát yêu cầu chuyển Scene.
    // Game lấy yêu cầu đó và xử lý.
    [[nodiscard]] std::optional<SceneRequest> consumeSceneRequest() override;

  private:
    // Helper
    void drawMainMenu();

    enum class MenuItem : std::uint8_t { Start, Settings, Exit };

    AssetManager& m_assetManager;
    AudioManager& m_audioManager;
    MenuItem m_selectedItem{MenuItem::Start};
    std::optional<SceneRequest> m_pendingSceneRequest;

    Font const* m_mainMenuItemsFont{};
    Font const* m_gamenameFont{};
    std::optional<Label> m_titleGameLbl;
    MenuList m_menuList;
};
