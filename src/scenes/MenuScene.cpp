#include "MenuScene.hpp"

#include "SceneRequest.hpp"
#include "core/GameConfig.hpp"
#include "ui/Label.hpp"
#include "ui/MenuList.hpp"

#include <optional>
#include <raylib.h>
#include <string>

void MenuScene::processInput() {
    int const result = m_menuList.processInputWithResult();
    if (result != MenuList::NO_SELECTION) {
        switch (result) {
            case 0: {
                m_pendingSceneRequest = SceneRequest{SceneId::Gameplay};
                break;
            }
            case 1: break;
            case 2: {
                m_pendingSceneRequest = SceneRequest{SceneId::Exit};
                break;
            }
            default: break;
        }
    }
}

void MenuScene::update(float deltaTime) {
    m_menuList.update(deltaTime);
}

void MenuScene::draw() const {
    // ClearBackground(Color{.r = 29, .g = 172, .b = 191});
    DrawRectangleGradientV(0,
                           0,
                           GameConfig::WINDOW_WIDTH,
                           GameConfig::WINDOW_HEIGHT,
                           Color{.r = 29, .g = 172, .b = 191},
                           Color{.r = 0, .g = 50, .b = 100, .a = 255});

    if (m_titleGameLbl) {
        m_titleGameLbl->draw();
    }

    m_menuList.draw();
}

std::optional<SceneRequest> MenuScene::consumeSceneRequest() {
    auto request = m_pendingSceneRequest;
    m_pendingSceneRequest.reset();
    return request;
}

void MenuScene::onEnter() {
    if (!m_assetManager.loadAssetGroup("menu")) {
        TraceLog(LOG_FATAL, "menu load failed");
        return;
    }

    m_gamenameFont = m_assetManager.font("gamename_font");
    m_titleGameLbl.emplace(
        Label::createCenteredLabel(*m_gamenameFont, "PONG", GameConfig::WINDOW_HEIGHT / 6.0F));
    m_titleGameLbl->setColor({.r = 29, .g = 121, .b = 191, .a = 255});

    m_mainMenuItemsFont = m_assetManager.font("main_menu_items_font");
    drawMainMenu();
}

void MenuScene::onExit() {
    m_assetManager.unloadAssetGroup("menu");
}

// Helper
void MenuScene::drawMainMenu() {
    m_menuList.addLabel(Label::createCenteredLabel(*m_mainMenuItemsFont, "START", 450.0F));
    m_menuList.addLabel(Label::createCenteredLabel(*m_mainMenuItemsFont, "SETTINGS", 500.0F));
    m_menuList.addLabel(Label::createCenteredLabel(*m_mainMenuItemsFont, "EXIT", 550.0F));

    m_menuList.onSelectionChanged([this]() {
        m_audioManager.playSound("select_item_sound");
    });
}
