#include "ui/Label.hpp"

#include "core/GameConfig.hpp"

#include <raylib.h>
#include <string>

void Label::draw() const {
    if (m_font == nullptr) {
        return;
    }

    DrawTextEx(*m_font,
               m_text.c_str(),
               m_position,
               static_cast<float>(m_font->baseSize),
               m_spacing,
               m_color);
}

float Label::calculateCenteredX(Font const& font, std::string const& text) {
    float const textWidth =
        MeasureTextEx(font, text.c_str(), static_cast<float>(font.baseSize), 2.0F).x;
    return (GameConfig::WINDOW_WIDTH / 2.0F) - (textWidth / 2.0F);
}

Label Label::createCenteredLabel(Font const& font, std::string const& text, float y) {
    return Label(font, text, Vector2{.x = calculateCenteredX(font, text), .y = y});
}
