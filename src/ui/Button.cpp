#include "ui/Button.hpp"

#include <raylib.h>
#include <string>

void Button::processInput() {
    Vector2 const mousePoint = GetMousePosition();
    bool const isMouseOver = CheckCollisionPointRec(mousePoint, m_bounds);

    if (!isMouseOver) {
        m_state = State::Normal;
        return;
    }

    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        m_state = State::Pressed;
    } else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        // Chuột nhấn vào rồi thả ra trên nút -> kích hoạt callback
        if (m_state == State::Pressed && m_callback) {
            m_callback();
        }
        m_state = State::Hovered;
    } else {
        m_state = State::Hovered;
    }
}

void Button::update(float deltaTime) {}

void Button::draw() const {
    if (m_font == nullptr) {
        return;
    }

    Color buttonColor{};
    Color textColor{};

    switch (m_state) {
        case State::Normal: {
            buttonColor = DARKGRAY;
            textColor = LIGHTGRAY;
            break;
        }
        case State::Hovered: {
            buttonColor = LIGHTGRAY;
            textColor = ORANGE;
            break;
        }
        case State::Pressed: {
            buttonColor = ORANGE;
            textColor = WHITE;
            break;
        }
    }

    // Vẽ nút
    DrawRectangleRec(m_bounds, buttonColor);
    // Vẽ viền nút
    DrawRectangleLinesEx(m_bounds, 2.0F, textColor);

    // Để text ở center của button
    Vector2 const textSize = MeasureTextEx(*m_font, m_text.c_str(), m_fontSize, m_spacing);

    float const textX = m_bounds.x + ((m_bounds.width - textSize.x) / 2.0F);
    float const textY = m_bounds.y + ((m_bounds.height - textSize.y) / 2.0F);

    DrawTextEx(
        *m_font, m_text.c_str(), Vector2{.x = textX, .y = textY}, m_fontSize, m_spacing, textColor);
}
