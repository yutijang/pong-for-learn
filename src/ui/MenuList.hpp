#pragma once

#include "IWidget.hpp"
#include "Label.hpp"

#include <cstddef>
#include <cstdint>
#include <functional>
#include <raylib.h>
#include <utility>
#include <vector>

class MenuList final : public IWidget {
  public:
    static constexpr int NO_SELECTION{-1};

    enum class Direction : std::uint8_t { Vertical, Horizontal };

    void processInput() override {}

    void update(float deltaTime) override;
    void draw() const override;

    // Chuyển kiểu trả về thành int để báo về Scene xem nút nào được nhấn
    // Trả về NO_SELECTION nếu người dùng chưa ấn ENTER
    int processInputWithResult();

    void addLabel(Label label) {
        m_items.push_back(std::move(label));
    }

    void setDirection(Direction direction) noexcept {
        m_direction = direction;
    }

    void setHasBullet(bool hasBullet) noexcept {
        m_hasBullet = hasBullet;
    }

    void onSelectionChanged(std::function<void()> callback) {
        m_onSelectionChangedCallback = std::move(callback);
    }

    void setColor(Color selectedColor, Color defaultColor = RAYWHITE) noexcept {
        m_selectedColor = selectedColor;
        m_defaultColor = defaultColor;
    }

  protected:
    void emitSelectionChanged() {
        if (m_onSelectionChangedCallback) {
            m_onSelectionChangedCallback();
        }
    }

  private:
    std::vector<Label> m_items;
    std::size_t m_selectedIndex{};
    Direction m_direction{Direction::Vertical};
    std::function<void()> m_onSelectionChangedCallback;
    Color m_selectedColor{ORANGE};
    Color m_defaultColor{RAYWHITE};
    bool m_hasBullet{};
};
