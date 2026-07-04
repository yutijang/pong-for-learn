#include "ui/PauseOverlay.hpp"

#include "core/GameConfig.hpp"
#include "ui/Label.hpp"
#include "ui/MenuList.hpp"

#include <raylib.h>
#include <string>

PauseOverlay::PauseOverlay(Font const* titleFont, Font const* itemFont)
    : m_titleFont(titleFont), m_itemFont(itemFont), m_itemList{} {
    m_itemList.setHasBullet(false);

    m_itemList.addLabel(Label::createCenteredLabel(*m_itemFont, GameConfig::Text::RESUME, 330.0F));
    m_itemList.addLabel(
        Label::createCenteredLabel(*m_itemFont, GameConfig::Text::BACK_TO_MAIN_MENU, 360.0F));

    m_titlePosition = {.x = Label::calculateCenteredX(*m_titleFont, GameConfig::Text::PAUSED),
                       .y = (GameConfig::WINDOW_HEIGHT / 2.0F) - 100.0F};
}

PauseOverlay::MenuResult PauseOverlay::processInput() {
    int const choice = m_itemList.processInputWithResult();

    if (choice == 0) {
        return MenuResult::Resume;
    }

    if (choice == 1) {
        return MenuResult::MainMenu;
    }

    return MenuResult::None;
}

void PauseOverlay::update(float deltaTime) {
    m_itemList.update(deltaTime);
}

void PauseOverlay::draw() const {
    // Vẽ lớp phủ kích thước lớn bằng cửa sổ game, màu xám đen có trong suốt
    DrawRectangle(0,
                  0,
                  GameConfig::WINDOW_WIDTH,
                  GameConfig::WINDOW_HEIGHT,
                  Color{.r = 20, .g = 20, .b = 20, .a = 200});

    // Vẽ tiêu đề
    DrawTextEx(*m_titleFont, GameConfig::Text::PAUSED, m_titlePosition, 65.0F, 2.0F, RAYWHITE);

    m_itemList.draw();
}
