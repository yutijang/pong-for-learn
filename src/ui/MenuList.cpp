#include "MenuList.hpp"

#include <cstddef>
#include <raylib.h>

void MenuList::update(float deltaTime) {
    for (std::size_t i{0}; i < m_items.size(); ++i) {
        auto& item = m_items[i];

        item.update(deltaTime);

        if (i == m_selectedIndex) {
            item.setColor(m_selectedColor);
        } else {
            item.setColor(m_defaultColor);
        }
    }
}

void MenuList::draw() const {
    for (std::size_t i{0}; i < m_items.size(); ++i) {
        auto const& item = m_items[i];

        item.draw();

        if (m_hasBullet && i == m_selectedIndex) {
            Vector2 const pos = item.position();
            DrawTextEx(item.font(),
                       "+",
                       Vector2{.x = pos.x - 25.0F, .y = pos.y - 2.0F},
                       item.fontSize(),
                       item.spacing(),
                       m_selectedColor);
        }
    }
}

int MenuList::processInputWithResult() {
    if (m_items.empty()) {
        return NO_SELECTION;
    }

    // Lưu lại chỉ số trước khi xử lý phím bấm
    std::size_t const oldIndex = m_selectedIndex;

    bool isNextPressed{};
    bool isPrevPressed{};

    if (m_direction == Direction::Vertical) {
        isNextPressed = IsKeyPressed(KEY_DOWN);
        isPrevPressed = IsKeyPressed(KEY_UP);
    } else {
        isNextPressed = IsKeyPressed(KEY_RIGHT);
        isPrevPressed = IsKeyPressed(KEY_LEFT);
    }

    if (isNextPressed) {
        if (m_selectedIndex < m_items.size() - 1) {
            m_selectedIndex++;
        } else {
            m_selectedIndex = 0; // Vòng lặp lại lên đầu/bên trái
        }
    }

    if (isPrevPressed) {
        if (m_selectedIndex > 0) {
            m_selectedIndex--;
        } else {
            m_selectedIndex = m_items.size() - 1; // Vòng lặp xuống cuối/bên phải
        }
    }

    // Kiểm tra xem chỉ số thực sự có thay đổi hay không để kích hoạt callback
    if (m_selectedIndex != oldIndex) {
        emitSelectionChanged();
    }

    // Nhấn ENTER: Trả về chỉ số của dòng đang chọn để Scene xử lý chuyển cảnh
    if (IsKeyPressed(KEY_ENTER)) {
        return static_cast<int>(m_selectedIndex);
    }

    return NO_SELECTION; // Chưa ấn ENTER
}
