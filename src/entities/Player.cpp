#include "Player.hpp"

#include <raylib.h>
#include <raymath.h>

void Player::processInput() {
    m_velocity = {.x = 0.0F, .y = 0.0F};

    if (IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN)) {
        m_velocity.y = -m_moveSpeed;
    } else if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_UP)) {
        m_velocity.y = m_moveSpeed;
    }
}

void Player::update(float deltaTime) {
    m_position = Vector2Add(m_position, Vector2Scale(m_velocity, deltaTime));
}

void Player::draw() const {
    DrawRectangleV(m_position, {.x = m_width, .y = m_height}, m_color);
}
