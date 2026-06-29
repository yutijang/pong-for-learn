#pragma once

#include "IWidget.hpp"

#include <raylib.h>
#include <string>
#include <string_view>
#include <utility>

class Label final : public IWidget {
  public:
    explicit Label(Font const& font, std::string text, Vector2 position, Color color = RAYWHITE)
        : m_font(&font), m_text(std::move(text)), m_position(position), m_color(color) {}

    void processInput() override {}

    void update(float deltaTime) override {}

    void draw() const override;

    void setColor(Color color) noexcept {
        m_color = color;
    }

    void setText(std::string_view text) noexcept {
        m_text = text;
    }

    void setSpacing(float spacing) noexcept {
        m_spacing = spacing;
    }

    void setPosition(Vector2 position) noexcept {
        m_position = position;
    }

    [[nodiscard]] Vector2 position() const noexcept {
        return m_position;
    }

    [[nodiscard]] Font font() const noexcept {
        return *m_font;
    }

    [[nodiscard]] float fontSize() const noexcept {
        return static_cast<float>(m_font->baseSize);
    }

    [[nodiscard]] float spacing() const noexcept {
        return m_spacing;
    }

    // Helper
    static float calculateCenteredX(Font const& font, std::string const& text);
    static Label createCenteredLabel(Font const& font, std::string const& text, float y);

  private:
    Font const* m_font{};
    std::string m_text;
    Vector2 m_position{};
    float m_spacing{2.0F};
    Color m_color;
};
