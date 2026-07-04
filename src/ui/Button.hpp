#pragma once

#include "ui/IWidget.hpp"

#include <cstdint>
#include <functional>
#include <raylib.h>
#include <string>
#include <utility>

class Button final : public IWidget {
  public:
    using Callback = std::function<void()>;

    enum class State : std::uint8_t { Normal, Hovered, Pressed };

    explicit Button(Font const& font, std::string text, Rectangle bounds, Callback callback = {})
        : m_font(&font), m_text(std::move(text)), m_bounds(bounds),
          m_callback(std::move(callback)) {}

    void processInput() override;
    void update(float deltaTime) override;
    void draw() const override;

    void setSelected(bool selected) noexcept {
        m_state = selected ? State::Hovered : State::Normal;
    }

  private:
    Font const* m_font{};
    std::string m_text;
    Rectangle m_bounds{};

    float m_fontSize{30.0F};
    float m_spacing{2.0F};

    State m_state{State::Normal};

    Callback m_callback;
};
