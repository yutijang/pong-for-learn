#pragma once

#include "ui/MenuList.hpp"

#include <cstdint>
#include <raylib.h>

class PauseOverlay {
  public:
    enum class MenuResult : std::uint8_t { None, Resume, MainMenu };

    explicit PauseOverlay(Font const* titleFont, Font const* itemFont);

    MenuResult processInput();
    void update(float deltaTime);
    void draw() const;

  private:
    MenuList m_itemList;
    Font const* m_titleFont{};
    Font const* m_itemFont{};
    Vector2 m_titlePosition{};
};
